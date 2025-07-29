#include "UsdViewport.h"

// --- CHANGE: Include QOpenGLFunctions for modern GL ---
#include <QOpenGLFunctions>

// --- USD/Hydra Includes ---
#include <pxr/usd/usd/stage.h>
#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hdx/tokens.h>
#include <pxr/imaging/hdx/types.h>
#include <pxr/imaging/hgi/hgi.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/tf/error.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/imaging/hd/rprimCollection.h>
#include <pxr/imaging/hd/renderIndex.h>
#include <pxr/imaging/hd/task.h>
#include <pxr/imaging/hdx/taskController.h>
#include <pxr/usdImaging/usdImaging/delegate.h>

PXR_NAMESPACE_USING_DIRECTIVE

// The PIMPL (Private Implementation) struct remains the same
struct UsdViewport::Impl {
    ~Impl() = default;
    UsdStageRefPtr stage;
    std::unique_ptr<Hgi> hgi;
    std::unique_ptr<HdEngine> engine;
    std::unique_ptr<HdxTaskController> taskController;
    HdRenderIndex* renderIndex = nullptr;
    UsdImagingDelegate* stageDelegate = nullptr;
};

// --- CHANGE: Constructor is much simpler ---
UsdViewport::UsdViewport(QWidget* parent)
    : QOpenGLWidget(parent), _impl(std::make_unique<Impl>()) {
    // No more setAttribute calls needed
    setFocusPolicy(Qt::StrongFocus);
}

UsdViewport::~UsdViewport() {
    // This function now needs to ensure the GL context is current
    // before destroying Hydra objects.
    makeCurrent();
    // Reset pointers in reverse order of creation
    _impl->stageDelegate = nullptr;
    _impl->taskController.reset();
    _impl->engine.reset();
    _impl->hgi.reset();
    if (_impl->renderIndex) {
        delete _impl->renderIndex;
        _impl->renderIndex = nullptr;
    }
    doneCurrent();
}

bool UsdViewport::loadStage(const std::string& filePath) {
    // This function can now be simpler. We just load the data and
    // request an update. The actual GL work happens in paintGL.
    if (!_impl->taskController) {
        return false; // Not initialized yet
    }

    try {
        _impl->stage = UsdStage::Open(filePath);
        if (!_impl->stage) {
            fprintf(stderr, "Error: Could not open stage %s\n", filePath.c_str());
            return false;
        }
        _impl->stageDelegate->Populate(_impl->stage->GetPseudoRoot());
        update(); // Request a repaint
    }
    catch (const TfError& e) {
        fprintf(stderr, "Error loading stage: %s\n", e.GetCommentary().c_str());
        return false;
    }
    return true;
}

// --- CHANGE: All GL and Hydra init logic moves here ---
void UsdViewport::initializeGL() {
    // This function is guaranteed by Qt to be called with a valid
    // and current OpenGL context.

    // We must initialize the Qt OpenGL function wrappers.
    initializeOpenGLFunctions();

    // Now we can safely initialize Hydra
    _impl->hgi = Hgi::CreatePlatformDefaultHgi();
    _impl->engine = std::make_unique<HdEngine>();

    const SdfPath controllerId = SdfPath::AbsoluteRootPath().AppendChild(TfToken("taskController"));
    _impl->renderIndex = HdRenderIndex::New(nullptr, {});
    _impl->taskController = std::make_unique<HdxTaskController>(_impl->renderIndex, controllerId);

    const SdfPath delegateId = SdfPath::AbsoluteRootPath().AppendChild(TfToken("stageDelegate"));
    _impl->stageDelegate = new UsdImagingDelegate(_impl->renderIndex, delegateId);
}

// --- CHANGE: All drawing logic moves here ---
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

    // Request the next frame
    update();
}

// --- CHANGE: Resize logic moves here ---
void UsdViewport::resizeGL(int w, int h) {
    // The viewport is set in paintGL, so this function is often empty
    // for simple cases. We'll leave it for now.
}