#ifndef USDVIEWPORT_H
#define USDVIEWPORT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <memory>
#include <string>

// Include USD namespace macros
#include <pxr/pxr.h>

// Forward-declare the USD classes with proper namespace
PXR_NAMESPACE_OPEN_SCOPE
class HdxTaskController;
class UsdImagingDelegate;
class UsdStage;
PXR_NAMESPACE_CLOSE_SCOPE

class UsdViewport : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit UsdViewport(QWidget* parent = nullptr);
    ~UsdViewport() override;

    bool loadStage(const std::string& filePath);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif // USDVIEWPORT_H