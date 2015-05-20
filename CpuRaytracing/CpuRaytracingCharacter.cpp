#include "CpuRaytracingCharacter.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>
#include <CellarWorkbench/Misc/StringUtils.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>
#include <PropRoom2D/Team/AbstractTeam.h>

#include <PropRoom3D/Prop/Prop.h>
#include <PropRoom3D/Prop/Volume/Sphere.h>
#include <PropRoom3D/Prop/Volume/Plane.h>
#include <PropRoom3D/Prop/Costume/Glass.h>
#include <PropRoom3D/Prop/Costume/Chrome.h>
#include <PropRoom3D/Prop/Costume/FlatPaint.h>
#include <PropRoom3D/Prop/Costume/TexturedPaint.h>
#include <PropRoom3D/Team/AbstractTeam.h>

#include <Scaena/Play/Play.h>
#include <Scaena/Play/View.h>
#include <Scaena/StageManagement/Event/StageTime.h>
#include <Scaena/StageManagement/Event/SynchronousKeyboard.h>
#include <Scaena/StageManagement/Event/SynchronousMouse.h>

using namespace cellar;
using namespace prop2;
using namespace prop3;
using namespace scaena;


CpuRaytracingCharacter::CpuRaytracingCharacter() :
    Character("CPU Raytracing Character")
{
}

void CpuRaytracingCharacter::enterStage()
{
    _fps = play().propTeam2D()->createTextHud();
    _fps->setText("FPS: ");
    _fps->setHandlePosition(glm::dvec2(18.0, -30.0));
    _fps->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _fps->setVerticalAnchor(EVerticalAnchor::TOP);
    _fps->setHeight(20);
    _fps->setIsVisible(false);

    _ups = play().propTeam2D()->createTextHud();
    _ups->setText("UPS: ");
    _ups->setHandlePosition(glm::dvec2(18.0, -50.0));
    _ups->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _ups->setVerticalAnchor(EVerticalAnchor::TOP);
    _ups->setHeight(20);
    _ups->setIsVisible(false);

    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(-1.2, -1.2, 5.25);
    glm::dvec3 camPos = focusPos + glm::dvec3(25, -40, 14) * 2.0;
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);


    glm::dvec3 negLim(-20.0, -20.0, 0.0);
    glm::dvec3 posLim( 20.0,  20.0, 0.0);
    glm::dvec3 socleDia = posLim - negLim;
    pVol xNeg(new Plane(glm::dvec3(-1, 0, 0), negLim));
    pVol xPos(new Plane(glm::dvec3( 1, 0, 0), posLim));
    pVol yNeg(new Plane(glm::dvec3( 0,-1, 0), negLim));
    pVol yPos(new Plane(glm::dvec3( 0, 1, 0), posLim));
    pVol zSoc(new PlaneTexture(glm::dvec3( 0, 0, 1), negLim,
              glm::dvec3(socleDia.x, 0.0, 0.0),
              glm::dvec3(0.0, socleDia.y, 0.0),
              negLim));
    pVol socle = (zSoc & ~(xNeg & xPos & yNeg & yPos));


    glm::dvec3 boxMin(-10.0, -10.0, 0.0);
    glm::dvec3 boxMax( 10.0,  10.0, 20.0);
    glm::dvec3 boxDia = boxMax - boxMin;
    pVol xBot(new Plane(glm::dvec3(-1,  0,  0), boxMin));
    pVol xTop(new Plane(glm::dvec3( 1,  0,  0), boxMax));
    pVol yTop(new Plane(glm::dvec3( 0,  1,  0), boxMax));
    pVol zTop(new Plane(glm::dvec3( 0,  0,  1), boxMax));
    pVol yBot(new PlaneTexture(
        glm::dvec3( 0, -1,  0), boxMin,
        glm::dvec3(boxDia.x, 0, 0),
        glm::dvec3(0, 0, boxDia.z),
        boxMin));
    pVol box = (xBot & yBot & xTop & yTop & zTop) & ~(!zSoc);

    glm::dvec3 thickness = glm::dvec3(0.5);
    glm::dvec3 boxCenter = glm::dvec3(0, 0, 10);
    glm::dvec3 pillard(boxMax.x, boxMin.y, boxMin.z);
    pVol sideWall(new Plane(glm::dvec3(1, 0, 0), boxCenter));
    pVol ceiling(new Plane(glm::dvec3( 0, 0, -1), boxMax - thickness));
    pVol pillarX(new Plane(glm::dvec3(-1, 0, 0), pillard - thickness));
    pVol pillarY(new Plane(glm::dvec3( 0, 1, 0), pillard + thickness));
    pVol zStep(new Plane(glm::dvec3(0, 0, 1), boxCenter));
    pVol yStep(new PlaneTexture(
       glm::dvec3( 0, -1, 0), boxCenter,
       glm::dvec3(boxDia.x, 0, 0),
       glm::dvec3(0, 0, boxDia.z),
       boxMin));
    pVol rearWall(new PlaneTexture(
       glm::dvec3( 0, -1, 0), boxMax - thickness,
       glm::dvec3(boxDia.x, 0, 0),
       glm::dvec3(0, 0, boxDia.z),
       boxMin));

    pVol stage = box &
        ((rearWall | ceiling | sideWall) |
         (pillarX & pillarY) |
         (yStep & zStep));

    pVol chromeBallSphere(
                new Sphere(glm::dvec3(5.0, -15.0, 2), 2.0));

    pVol glassBallBase = pVol(new Sphere(glm::dvec3(15, -5, 2.7), 3.0));
    pVol glassBallSphere = glassBallBase &
        !(pVol(new Sphere(glm::dvec3(15, -5, 2.7), 2.3)) &
          pVol(new Plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.7))))&
        pVol(new Plane(glm::dvec3(0,0,1), glm::dvec3(0,0,2.7))) &
        pVol(new Plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.05)));

    std::shared_ptr<prop3::Costume> socleCostume(
                new TexturedPaint(":/CpuRaytracing/Bathroom_Tiles.png"));
    std::shared_ptr<prop3::Costume> stageCostume(
                new TexturedPaint(":/CpuRaytracing/Fusion_Albums.png",
                                  glm::dvec3(0.7, 0.7, 0.7)));
    std::shared_ptr<prop3::Costume> glassBallCostume(
                new Glass(glm::dvec3(0.95, 0.75, 0.72)));
    std::shared_ptr<prop3::Costume> chromeBallCostume(
                new Chrome(glm::dvec3(212, 175, 55) / 255.0));

    _socle = play().propTeam3D()->createProp();
    _socle->setCostume(socleCostume);
    _socle->setVolume(socle);

    _stage = play().propTeam3D()->createProp();
    _stage->setCostume(stageCostume);
    //_stage->setBoundingVolume(box);
    _stage->setVolume(stage);

    _bowl = play().propTeam3D()->createProp();
    _bowl->setCostume(glassBallCostume);
    _bowl->setBoundingVolume(glassBallBase);
    _bowl->setVolume(glassBallSphere);

    _ball = play().propTeam3D()->createProp();
    _ball->setCostume(chromeBallCostume);
    _ball->setVolume(chromeBallSphere);
}

void CpuRaytracingCharacter::beginStep(const StageTime &time)
{
    float elapsedtime = time.elapsedTime();
    _ups->setText("UPS: " + toString(floor(1.0 / elapsedtime)));

    float velocity  = 16.0f * elapsedtime;
    const float turnSpeed = 0.004f;

    SynchronousMouse& syncMouse = *play().synchronousMouse();
    SynchronousKeyboard& syncKeyboard = *play().synchronousKeyboard();

    if(syncKeyboard.isAsciiPressed('w'))
    {
        _camMan->forward(velocity);
    }
    if(syncKeyboard.isAsciiPressed('s'))
    {
        _camMan->forward(-velocity);
    }
    if(syncKeyboard.isAsciiPressed('a'))
    {
        _camMan->sideward(-velocity);
    }
    if(syncKeyboard.isAsciiPressed('d'))
    {
        _camMan->sideward(velocity);
    }

    if(syncMouse.displacement() != glm::ivec2(0, 0) &&
       syncMouse.buttonIsPressed(EMouseButton::LEFT))
    {
        _camMan->pan( syncMouse.displacement().x * -turnSpeed);
        _camMan->tilt(syncMouse.displacement().y * -turnSpeed);
    }
}

void CpuRaytracingCharacter::draw(const std::shared_ptr<scaena::View> &,
                                  const scaena::StageTime&time)
{
    _fps->setText("FPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void CpuRaytracingCharacter::exitStage()
{
    play().propTeam2D()->deleteTextHud(_fps);
    play().propTeam2D()->deleteTextHud(_ups);
}
