#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <memory>

#include <QWidget>

#include <CellarWorkbench/Camera/Camera.h>

namespace Ui
{
    class RaytracerGui;
}


class CameraManager : public QObject
{
public:
    CameraManager(Ui::RaytracerGui* ui);
    virtual ~CameraManager();

    virtual void setView(QWidget* view);
    virtual void setCamera(const std::shared_ptr<cellar::Camera>& camera);

protected slots:
    virtual void captureDimensionsChanged(int unused);
    virtual void outputDimensionsChanged(int unused);
    virtual void outputMatchCaptureDimensions(bool match);
    virtual void fieldOfViewChanged(int unused);
    virtual void enterFullscreen();
    virtual void exitFullscreen();

protected:
    virtual void updateCameraProjection();
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::RaytracerGui* _ui;
    QWidget* _view;
    QWidget* _fullscreenWindow;
    std::shared_ptr<cellar::Camera> _camera;
};

#endif // CAMERAMANAGER_H
