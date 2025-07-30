#include "UsdViewport.h"

#include <QOpenGLFunctions>

// USD/Hydra Includes - IMPORTANT: Include pxr.h first
#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hd/renderDelegate.h>  // Pour HdRenderSettingsMap
#include <pxr/imaging/hd/driver.h>          // Pour HdDriver
#include <pxr/imaging/hd/tokens.h>          // Pour HdTokens
#include <pxr/imaging/hdx/tokens.h>
#include <pxr/imaging/hdx/types.h>
#include <pxr/imaging/hgi/hgi.h>
#include <pxr/imaging/hgiGL/hgi.h>  // Pour HgiGL
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/tf/error.h>
#include <pxr/base/vt/value.h>              // Pour VtValue
#include <pxr/usd/sdf/path.h>
#include <pxr/imaging/hd/rprimCollection.h>
#include <pxr/imaging/hd/renderIndex.h>
#include <pxr/imaging/hd/task.h>
#include <pxr/imaging/hdx/taskController.h>
#include <pxr/usdImaging/usdImaging/delegate.h>
#include <pxr/imaging/hdSt/renderDelegate.h>  // Storm GL render delegate

// Use the PXR namespace
PXR_NAMESPACE_USING_DIRECTIVE

struct UsdViewport::Impl {
    ~Impl() = default;
    UsdStageRefPtr stage;
    std::unique_ptr<Hgi> hgi;
    std::unique_ptr<HdEngine> engine;
    std::unique_ptr<HdxTaskController> taskController;
    HdRenderIndex* renderIndex = nullptr;
    UsdImagingDelegate* stageDelegate = nullptr;
    std::unique_ptr<HdStRenderDelegate> renderDelegate;  // Storm GL render delegate
    std::unique_ptr<HdDriver> hgiDriverPtr;              // Stockage du driver pour sa durée de vie
};

UsdViewport::UsdViewport(QWidget* parent)
    : QOpenGLWidget(parent), _impl(std::make_unique<Impl>()) {
    setFocusPolicy(Qt::StrongFocus);
}

UsdViewport::~UsdViewport() {
    makeCurrent();
    // Reset in reverse order of creation
    if (_impl->stageDelegate) {
        delete _impl->stageDelegate;
        _impl->stageDelegate = nullptr;
    }
    _impl->taskController.reset();
    _impl->engine.reset();
    if (_impl->renderIndex) {
        delete _impl->renderIndex;
        _impl->renderIndex = nullptr;
    }
    _impl->renderDelegate.reset();
    _impl->hgiDriverPtr.reset();  // Nettoie le driver
    _impl->hgi.reset();
    doneCurrent();
}

bool UsdViewport::loadStage(const std::string& filePath) {
    if (!_impl->taskController) {
        return false;
    }

    try {
        _impl->stage = UsdStage::Open(filePath);
        if (!_impl->stage) {
            fprintf(stderr, "Error: Could not open stage %s\n", filePath.c_str());
            return false;
        }
        _impl->stageDelegate->Populate(_impl->stage->GetPseudoRoot());
        update();
    }
    catch (const std::exception& e) {
        fprintf(stderr, "Error loading stage: %s\n", e.what());
        return false;
    }
    return true;
}

void UsdViewport::initializeGL() {
    initializeOpenGLFunctions();

    // Initialize Hgi (Hardware Graphics Interface)
    _impl->hgi = Hgi::CreatePlatformDefaultHgi();

    // Create HdDriver with our Hgi instance
    HdDriverVector drivers;
    auto hgiDriver = std::make_unique<HdDriver>();
    hgiDriver->name = TfToken("HdStormRenderDelegate");  // Utilise un token générique
    hgiDriver->driver = VtValue(_impl->hgi.get());
    drivers.push_back(hgiDriver.get());

    // Create Storm GL render delegate with empty settings map
    HdRenderSettingsMap settingsMap;
    _impl->renderDelegate = std::make_unique<HdStRenderDelegate>(settingsMap);

    // Create render index with the render delegate and drivers
    _impl->renderIndex = HdRenderIndex::New(_impl->renderDelegate.get(), drivers);

    // Store the driver to keep it alive
    _impl->hgiDriverPtr = std::move(hgiDriver);

    // Create Hydra engine
    _impl->engine = std::make_unique<HdEngine>();

    // Create task controller
    const SdfPath controllerId = SdfPath::AbsoluteRootPath().AppendChild(TfToken("taskController"));
    _impl->taskController = std::make_unique<HdxTaskController>(_impl->renderIndex, controllerId);

    // Create USD imaging delegate
    const SdfPath delegateId = SdfPath::AbsoluteRootPath().AppendChild(TfToken("stageDelegate"));
    _impl->stageDelegate = new UsdImagingDelegate(_impl->renderIndex, delegateId);
}

void UsdViewport::paintGL() {
    if (!_impl->taskController) {
        return;
    }

    const TfToken geometryCollectionName("geometry");
    HdRprimCollection collection(geometryCollectionName, HdReprSelector(HdReprTokens->hull));
    _impl->taskController->SetCollection(collection);

    HdxRenderTaskParams renderParams;
    renderParams.camera = SdfPath::AbsoluteRootPath();
    renderParams.viewport = GfVec4f(0, 0, width(), height());
    _impl->taskController->SetRenderParams(renderParams);

    HdTaskSharedPtrVector tasks = _impl->taskController->GetRenderingTasks();
    _impl->engine->Execute(_impl->renderIndex, &tasks);

    update();
}

void UsdViewport::resizeGL(int w, int h) {
    // Viewport is set in paintGL
}