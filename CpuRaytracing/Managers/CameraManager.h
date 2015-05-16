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

private:
    Ui::RaytracerGui* _ui;
    QWidget* _view;
    std::shared_ptr<cellar::Camera> _camera;
};

#endif // CAMERAMANAGER_H
