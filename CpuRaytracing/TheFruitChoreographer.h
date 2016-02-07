#ifndef THEFRUIT_CHOREOGRAPHER_H
#define THEFRUIT_CHOREOGRAPHER_H

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


class TheFruitChoreographer : public prop3::AbstractChoreographer
{
public:
    TheFruitChoreographer(const std::shared_ptr<cellar::Camera>& camera,
                          const std::shared_ptr<prop3::RaytracerState>& raytracerState);
    virtual ~TheFruitChoreographer();


    virtual void setup(const std::shared_ptr<prop3::StageSet>& stageSet) override;
    virtual void update(double dt) override;
    virtual void terminate() override;
    virtual void restart() override;


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
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _cameraEyePath;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _cameraToPath;
    std::shared_ptr<cellar::AbstractPath<double>> _cameraFoV;
    double _camAperture;

    glm::dvec3 _cloudsVelocity;
    std::shared_ptr<prop3::StageZone> _cloudsZone;

    glm::dvec3 _theFruitPosition;
    std::shared_ptr<prop3::Prop> _theFruitProp;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> _theFruitPath;

    std::shared_ptr<prop3::ProceduralSun> _backdrop;
    std::shared_ptr<cellar::AbstractPath<double>> _sunPath;

    std::shared_ptr<prop3::RaytracerState> _raytracerState;
    double _animTime;
    double _animFps;
};

#endif // THEFRUIT_CHOREOGRAPHER_H
