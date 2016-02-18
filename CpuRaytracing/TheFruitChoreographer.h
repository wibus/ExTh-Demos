#ifndef THEFRUIT_CHOREOGRAPHER_H
#define THEFRUIT_CHOREOGRAPHER_H

#include <QObject>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>

#include <CellarWorkbench/Path/AbstractPath.h>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/Choreographer/AbstractChoreographer.h>

#include <PropRoom3D/Node/StageSet.h>
#include <PropRoom3D/Prop/Prop.h>
#include <PropRoom3D/Prop/Surface/Box.h>
#include <PropRoom3D/Prop/Surface/Sphere.h>
#include <PropRoom3D/Prop/Surface/Plane.h>
#include <PropRoom3D/Prop/Surface/Quadric.h>
#include <PropRoom3D/Prop/Material/UniformStdMaterial.h>
#include <PropRoom3D/Prop/Coating/UniformStdCoating.h>
#include <PropRoom3D/Prop/Coating/TexturedStdCoating.h>
#include <PropRoom3D/Light/Backdrop/ProceduralSun.h>
#include <PropRoom3D/Light/LightBulb/CircularLight.h>
#include <PropRoom3D/Light/LightBulb/SphericalLight.h>

class PathManager;


class TheFruitChoreographer : public QObject, public prop3::AbstractChoreographer
{
    Q_OBJECT

public:
    TheFruitChoreographer(const std::shared_ptr<cellar::Camera>& camera,
                          const std::shared_ptr<prop3::RaytracerState>& raytracerState);
    virtual ~TheFruitChoreographer();


    virtual void setup(const std::shared_ptr<prop3::StageSet>& stageSet) override;
    virtual void update(double dt) override;
    virtual void terminate() override;

    virtual int animFrameCount();
    virtual void setAnimFps(int fps);
    virtual void setAnimFrame(int frame);
    virtual void resetAnimation();
    virtual void playAnimation();
    virtual void pauseAnimation();
    virtual void setFastPlay(bool playFast);

    virtual void displayPaths(PathManager& pathManager);


signals:
    void animFrameChanged(int frameId);
    void playStateChanged(bool isPlaying);


protected:
    virtual void setupAnimation();
    virtual std::shared_ptr<prop3::Surface> createHoleStrippedWall(
            const glm::dvec3& size,
            double stripeWidth,
            double holeWidth,
            double border);

private:
    std::shared_ptr<cellar::Camera> _camera;
    std::shared_ptr<cellar::CameraManFree> _camMan;
    double _camAperture;

    glm::dvec3 _cloudsPosition;
    std::shared_ptr<prop3::StageZone> _cloudsZone;

    glm::dvec3 _theFruitPosition;
    std::shared_ptr<prop3::Prop> _theFruitProp;
    std::shared_ptr<prop3::ProceduralSun> _backdrop;

    std::shared_ptr<prop3::RaytracerState> _raytracerState;

    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _cameraEyePath;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _cameraToPath;
    std::shared_ptr<cellar::AbstractPath<double>> _cameraFoV;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _theFruitPath;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _cloudsPath;
    std::shared_ptr<cellar::AbstractPath<double>> _sunPath;

    int _animFps;
    int _animFrame;
    double _animTime;
    bool _animPlaying;
    bool _animFastPlay;
    double _animTotalTime;
};

#endif // THEFRUIT_CHOREOGRAPHER_H
