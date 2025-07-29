#ifndef USDVIEWPORT_H
#define USDVIEWPORT_H


#include <QOpenGLWidget> // <--- CHANGE: Include QOpenGLWidget
#include <QOpenGLFunctions>
#include <memory>
#include <string>

#include <pxr/pxr.h>

// Forward-declare the USD classes
PXR_NAMESPACE_OPEN_SCOPE
class HdxTaskController;
class UsdImagingDelegate;
class UsdStage;
PXR_NAMESPACE_CLOSE_SCOPE

// --- CHANGE: Inherit from QOpenGLWidget ---
class UsdViewport : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit UsdViewport(QWidget* parent = nullptr);
    ~UsdViewport() override;

    bool loadStage(const std::string& filePath);

protected:
    // --- CHANGE: Override QOpenGLWidget's virtual functions ---
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    // We no longer need _initializeHydra or _draw private methods
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif // USDVIEWPORT_H