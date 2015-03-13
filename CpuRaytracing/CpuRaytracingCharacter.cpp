#include "CpuRaytracingCharacter.h"

#include <CellarWorkbench/Misc/CellarUtils.h>
using namespace cellar;

#include <PropRoom2D/PropTeam/AbstractPropTeam.h>
#include <PropRoom2D/Hud/TextHud.h>
using namespace prop2;

#include <PropRoom3D/Team/AbstractTeam.h>
#include <PropRoom3D/Prop/Prop.h>
#include <PropRoom3D/Prop/Volume/Sphere.h>
#include <PropRoom3D/Prop/Volume/Plane.h>
#include <PropRoom3D/Prop/Costume/Glass.h>
#include <PropRoom3D/Prop/Costume/Chrome.h>
#include <PropRoom3D/Prop/Costume/FlatPaint.h>
#include <PropRoom3D/Prop/Costume/TexturedPaint.h>
using namespace prop3;

#include <Scaena/Stage/AbstractStage.h>
#include <Scaena/Stage/Event/StageTime.h>
using namespace scaena;


CpuRaytracingCharacter::CpuRaytracingCharacter(AbstractStage &stage) :
    AbstractCharacter(stage, "CPU Raytracing Character")
{
}

void CpuRaytracingCharacter::enterStage()
{
    _fps = stage().propTeam2D().createTextHud();
    _fps->setText("FPS: ");
    _fps->setHandlePosition(glm::dvec2(18.0, -30.0));
    _fps->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _fps->setVerticalAnchor(EVerticalAnchor::TOP);
    _fps->setHeight(20);
    _fps->setIsVisible(false);

    _ups = stage().propTeam2D().createTextHud();
    _ups->setText("UPS: ");
    _ups->setHandlePosition(glm::dvec2(18.0, -50.0));
    _ups->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _ups->setVerticalAnchor(EVerticalAnchor::TOP);
    _ups->setHeight(20);
    _ups->setIsVisible(false);


    glm::dvec3 negLim(-20.0, -20.0, 0.0);
    glm::dvec3 posLim( 20.0,  20.0, 0.0);
    std::shared_ptr<Volume> xNeg(new PlaneGhost(glm::dvec3(-1, 0, 0), negLim));
    std::shared_ptr<Volume> xPos(new PlaneGhost(glm::dvec3( 1, 0, 0), posLim));
    std::shared_ptr<Volume> yNeg(new PlaneGhost(glm::dvec3( 0,-1, 0), negLim));
    std::shared_ptr<Volume> yPos(new PlaneGhost(glm::dvec3( 0, 1, 0), posLim));
    std::shared_ptr<Volume> zSoc(new Plane(glm::dvec3( 0, 0, 1), negLim));
    std::shared_ptr<Volume> socle = (zSoc & xNeg & xPos & yNeg & yPos);


    glm::dvec3 boxMin(-10.0, -10.0, 0.0);
    glm::dvec3 boxMax( 10.0,  10.0, 20.0);
    glm::dvec3 boxDia = boxMax - boxMin;
    std::shared_ptr<Volume> xBot(new Plane(glm::dvec3(-1,  0,  0), boxMin));
    std::shared_ptr<Volume> xTop(new Plane(glm::dvec3( 1,  0,  0), boxMax));
    std::shared_ptr<Volume> yTop(new Plane(glm::dvec3( 0,  1,  0), boxMax));
    std::shared_ptr<Volume> zTop(new Plane(glm::dvec3( 0,  0,  1), boxMax));
    std::shared_ptr<Volume> yBot(new PlaneTexture(
        glm::dvec3( 0, -1,  0), boxMin,
        glm::dvec3(boxDia.x, 0, 0),
        glm::dvec3(0, 0, boxDia.z),
        boxMin));
    std::shared_ptr<Volume> box = (xBot & yBot & xTop & yTop & zTop);

    glm::dvec3 thickness = glm::dvec3(0.5);
    glm::dvec3 boxCenter = glm::dvec3(0, 0, 10);
    glm::dvec3 pillard(boxMax.x, boxMin.y, boxMin.z);
    std::shared_ptr<Volume> xPlane(new Plane(glm::dvec3(1, 0, 0), boxCenter));
    std::shared_ptr<Volume> zPlane(new Plane(glm::dvec3(0, 0, 1), boxCenter));
    std::shared_ptr<Volume> roof(new Plane(glm::dvec3( 0, 0, -1), boxMax - thickness));
    std::shared_ptr<Volume> pillarX(new Plane(glm::dvec3(-1, 0, 0), pillard - thickness));
    std::shared_ptr<Volume> pillarY(new Plane(glm::dvec3( 0, 1, 0), pillard + thickness));
    std::shared_ptr<Volume> yPlane(new PlaneTexture(
       glm::dvec3( 0, -1, 0), boxCenter,
       glm::dvec3(boxDia.x, 0, 0),
       glm::dvec3(0, 0, boxDia.z),
       boxMin));
    std::shared_ptr<Volume> rearWall(new PlaneTexture(
       glm::dvec3( 0, -1, 0), boxMax - thickness,
       glm::dvec3(boxDia.x, 0, 0),
       glm::dvec3(0, 0, boxDia.z),
       boxMin));

    std::shared_ptr<Volume> crop =
        (rearWall | roof | (pillarX & pillarY) | xPlane | (yPlane & zPlane));

    std::shared_ptr<Volume> chromeBallSphere(
                new Sphere(glm::dvec3(5.0, -5.0, -8.0) + boxCenter, 2.0));
    std::shared_ptr<Volume> glassBallSphere =
        std::shared_ptr<Volume>(new Sphere(glm::dvec3(5.0, 2.0, 4.0) + boxCenter, 4.0)) &
        std::shared_ptr<Volume>(new Sphere(glm::dvec3(5.0, 8.0, 4.0) + boxCenter, 4.0));

    std::shared_ptr<prop3::Costume> wallsCostume(
                new TexturedPaint(":/CpuRaytracing/Fusion_Albums.png",
                                  glm::dvec3(0.4, 0.4, 0.4)));
    std::shared_ptr<prop3::Costume> glassBallCostume(
                new Glass());
    std::shared_ptr<prop3::Costume> chromeBallCostume(
                new Chrome(glm::dvec3(212, 175, 55) / 255.0));

    _walls = stage().propTeam3D().createProp();
    _walls->setVolume(socle | (box & crop));
    _walls->setCostume(wallsCostume);

    _glassLens = stage().propTeam3D().createProp();
    _glassLens->setVolume(glassBallSphere);
    _glassLens->setCostume(glassBallCostume);

    _chromeBall = stage().propTeam3D().createProp();
    _chromeBall->setVolume(chromeBallSphere);
    _chromeBall->setCostume(chromeBallCostume);
}

void CpuRaytracingCharacter::beginStep(const StageTime &time)
{
    _ups->setText("UPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void CpuRaytracingCharacter::draw(const StageTime &time)
{
    _fps->setText("FPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void CpuRaytracingCharacter::exitStage()
{
    stage().propTeam2D().deleteTextHud(_fps);
    stage().propTeam2D().deleteTextHud(_ups);
}
