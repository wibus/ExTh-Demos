#include "CpuRaytracingCharacter.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>
#include <CellarWorkbench/Misc/StringUtils.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>
#include <PropRoom2D/Team/AbstractTeam.h>

#include <PropRoom3D/Prop/Prop.h>
#include <PropRoom3D/Prop/ImplicitSurface/Sphere.h>
#include <PropRoom3D/Prop/ImplicitSurface/Plane.h>
#include <PropRoom3D/Prop/ImplicitSurface/Quadric.h>
#include <PropRoom3D/Prop/Material/Glass.h>
#include <PropRoom3D/Prop/Material/Chrome.h>
#include <PropRoom3D/Prop/Coating/FlatPaint.h>
#include <PropRoom3D/Prop/Coating/GlossyPaint.h>
#include <PropRoom3D/Prop/Coating/TexturedFlatPaint.h>
#include <PropRoom3D/Prop/Coating/TexturedGlossyPaint.h>
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


typedef std::shared_ptr<ImplicitSurface> pSurf;
typedef std::shared_ptr<Material> pMat;
typedef std::shared_ptr<Coating> pCoat;


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

    // Choose and setup scene
    //setupSatgeScene();
    //setupConvergenceScene();
    setupQuadricScene();
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

    for(auto& prop : _props)
    {
        play().propTeam3D()->deleteProp(prop);
    }
}

void CpuRaytracingCharacter::setupSatgeScene()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(-1.2, -1.2, 5.25);
    glm::dvec3 camPos = focusPos + glm::dvec3(25, -40, 14) * 2.1;
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);



    // Socle
    glm::dvec3 negLim(-20.0, -20.0, 0.0);
    glm::dvec3 posLim( 20.0,  20.0, 0.0);
    glm::dvec3 socleDia = posLim - negLim;
    pSurf xNeg = Plane::plane(glm::dvec3(-1, 0, 0), negLim);
    pSurf yPos = Plane::plane(glm::dvec3( 0, 1, 0), posLim);
    pSurf zNeg = Plane::plane(glm::dvec3( 0, 0,-1), negLim + glm::dvec3(0, 0, -0.8));
    pSurf xPos = PlaneTexture::plane(glm::dvec3( 1, 0, 0), posLim,
               glm::dvec3(socleDia.x, 0.0, 0.0),
               glm::dvec3(0.0, socleDia.y, 0.0),
               negLim);
    pSurf yNeg = PlaneTexture::plane(glm::dvec3( 0,-1, 0), negLim,
               glm::dvec3(socleDia.x, 0.0, 0.0),
               glm::dvec3(0.0, socleDia.y, 0.0),
               negLim);
    pSurf zSoc = PlaneTexture::plane(glm::dvec3( 0, 0, 1), negLim,
               glm::dvec3(socleDia.x, 0.0, 0.0),
               glm::dvec3(0.0, socleDia.y, 0.0),
               negLim);
    pSurf socleSide = xPos & yNeg;
    pSurf socleTop = (zSoc & ~xNeg & ~yPos & ~zNeg);
    pSurf socleSurf = socleTop & socleSide;

    pCoat socleTopCoat(new TexturedGlossyPaint(
            ":/CpuRaytracing/Bathroom_Tiles_albedo.png",
            ":/CpuRaytracing/Bathroom_Tiles_gloss.png",
            glm::dvec3(1.0, 1.0, 1.0)));

    pCoat socleSideCoat(new FlatPaint(
            glm::dvec3(0.5, 0.5, 0.5)));

    std::shared_ptr<Prop> socle = play().propTeam3D()->createProp();
    socleTop->setCoating(socleTopCoat);
    socleSide->setCoating(socleSideCoat);
    socle->setSurface(socleSurf);
    _props.push_back(socle);


    // Stage
    glm::dvec3 boxMin(-10.0, -10.0, 0.0);
    glm::dvec3 boxMax( 10.0,  10.0, 20.0);
    glm::dvec3 boxDia = boxMax - boxMin;
    glm::dvec3 boxCenter = boxMin + boxDia / 2.0;
    glm::dvec3 thickness = glm::dvec3(0.5);
    glm::dvec3 pillard(boxMax.x, boxMin.y, boxMin.z);
    pSurf xBot = Plane::plane(glm::dvec3(-1,  0,  0), boxMin);
    pSurf xTop = Plane::plane(glm::dvec3( 1,  0,  0), boxMax);
    pSurf yTop = Plane::plane(glm::dvec3( 0,  1,  0), boxMax);
    pSurf zTop = Plane::plane(glm::dvec3( 0,  0,  1), boxMax);
    pSurf yBot = PlaneTexture::plane(
        glm::dvec3( 0, -1,  0), boxMin,
        glm::dvec3(boxDia.x, 0, 0),
        glm::dvec3(0, 0, boxDia.z),
        boxMin);
    pSurf box = (xBot & yBot & xTop & yTop & zTop) & ~(!zSoc);
    pSurf sideWall = Plane::plane(glm::dvec3(1, 0, 0), boxCenter);
    pSurf ceiling = Plane::plane(glm::dvec3( 0, 0, -1), boxMax - thickness);
    pSurf pillarX = Plane::plane(glm::dvec3(-1, 0, 0), pillard - thickness);
    pSurf pillarY = Plane::plane(glm::dvec3( 0, 1, 0), pillard + thickness);
    pSurf zStep = Plane::plane(glm::dvec3(0, 0, 1), boxCenter);
    pSurf yStep = PlaneTexture::plane(
       glm::dvec3( 0, -1, 0), boxCenter,
       glm::dvec3(boxDia.x, 0, 0),
       glm::dvec3(0, 0, boxDia.z),
       boxMin);
    pSurf rearWall = PlaneTexture::plane(
       glm::dvec3( 0, -1, 0), boxMax - thickness,
       glm::dvec3(boxDia.x, 0, 0),
       glm::dvec3(0, 0, boxDia.z),
       boxMin);

    pSurf stageSurf = box &
        ((rearWall | ceiling | sideWall) |
         (pillarX & pillarY) |
         (yStep & zStep));

    pCoat posterCoat(new TexturedFlatPaint(
            ":/CpuRaytracing/Fusion_Albums.png",
            glm::dvec3(0.7, 0.7, 0.7)));

    std::shared_ptr<Prop> stage = play().propTeam3D()->createProp();
    stageSurf->setCoating(posterCoat);
    stage->setSurface(stageSurf);
    _props.push_back(stage);



    // Bowl
    pSurf bowlBase = Sphere::sphere(glm::dvec3(15, -5, 2.7), 3.0);
    pSurf bowlSurf = bowlBase &
        !(Sphere::sphere(glm::dvec3(15, -5, 2.7), 2.6) &
          Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.5)))&
        Plane::plane(glm::dvec3(0,0,1), glm::dvec3(0,0,2.7)) &
        Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.1));
    pMat bowlMat(new Glass(glm::dvec3(0.95, 0.75, 0.72), 6.0));

    std::shared_ptr<Prop> bowl = play().propTeam3D()->createProp();
    bowl->setBoundingSurface(bowlBase);
    bowl->setSurface(bowlSurf);
    bowl->setMaterial(bowlMat);
    _props.push_back(bowl);



    // Ball
    pSurf ballSurf = Sphere::sphere(glm::dvec3(5.0, -15.0, 2), 2.0);
    pMat ballMat(new Chrome(glm::dvec3(212, 175, 55) / 255.0));
    std::shared_ptr<Prop> ball = play().propTeam3D()->createProp();
    ball->setSurface(ballSurf);
    ball->setMaterial(ballMat);
    _props.push_back(ball);
}

void CpuRaytracingCharacter::setupConvergenceScene()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 0);
    glm::dvec3 camPos = glm::dvec3(0, 5, 0);
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);

    // Wall
    pSurf wallPlane = Plane::plane(glm::dvec3(0, 1, 0), glm::dvec3(0, 0, 0));
    pCoat wallPaint(new FlatPaint(glm::dvec3(1, 1, 1)));
    std::shared_ptr<Prop> wall = play().propTeam3D()->createProp();
    wallPlane->setCoating(wallPaint);
    wall->setSurface(wallPlane);
    _props.push_back(wall);
}

void CpuRaytracingCharacter::setupQuadricScene()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 2);
    glm::dvec3 camPos = glm::dvec3(20, 8, 8);
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);

    // Wall (cylinder)
    double filletRadius = 1.0;
    glm::dvec3 wallOrig(-3, 0, 0);
    glm::dvec3 filletOrig = wallOrig + glm::dvec3(filletRadius, 0 , filletRadius);
    pSurf floorPlane = Plane::plane(glm::dvec3(0, 0, 1), wallOrig);
    pSurf wallPlane = Plane::plane(glm::dvec3(1, 0, 0), wallOrig);
    pSurf xPos = Plane::plane(glm::dvec3(1, 0, 0), glm::dvec3());
    pSurf zPos = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3());
    pSurf cylinder = Quadric::cylinder(filletRadius, filletRadius);
    cylinder->transform(glm::rotate(glm::dmat4(), glm::pi<double>()/2, glm::dvec3(1, 0, 0)));
    pSurf fillet = cylinder & ~(xPos & zPos);
    fillet->transform(glm::translate(glm::dmat4(), filletOrig));

    pCoat envPaint(new FlatPaint(glm::dvec3(0.8, 0.8, 0.8)));
    pSurf envSurf = floorPlane | wallPlane | fillet;
    std::shared_ptr<Prop> envProp = play().propTeam3D()->createProp();
    envSurf->setCoating(envPaint);
    envProp->setSurface(envSurf);
    _props.push_back(envProp);

    // Paraboloid
    pSurf paraBase = Quadric::paraboloid(0.5, 0.5);
    pSurf paraTopCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 2));
    pSurf paraSurf = paraBase & paraTopCap;
    paraSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(0.1, -3, 0.5)));
    pCoat paraCoat(new GlossyPaint(glm::dvec3(0.90, 0.90, 0.30), 0.15));
    std::shared_ptr<Prop> paraProp = play().propTeam3D()->createProp();
    paraProp->setSurface(paraSurf);
    paraSurf->setCoating(paraCoat);
    _props.push_back(paraProp);

    // Egg (ellipsoid)
    pSurf eggTop = Quadric::ellipsoid(1, 1, 2);
    eggTop->transform(glm::translate(glm::dmat4(), glm::dvec3(0, 0, 0)));
    pSurf eggBottom = Quadric::ellipsoid(1, 1, 1);
    pSurf topCap = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    pSurf bottomCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf eggSurf = (eggTop & ~topCap) | (eggBottom & ~bottomCap);
    eggSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(0, 0, 1)));
    pCoat eggCoat(new GlossyPaint(glm::dvec3(0.90, 0.35, 0.10), 0.4));
    std::shared_ptr<Prop> eggProp = play().propTeam3D()->createProp();
    eggProp->setSurface(eggSurf);
    eggSurf->setCoating(eggCoat);
    _props.push_back(eggProp);

    // Cone
    pSurf coneBase = Quadric::cone(0.5, 0.5);
    pSurf coneTopCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3());
    pSurf coneBottomCap = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, -2));
    pSurf coneSurf = coneBase & coneTopCap & coneBottomCap;
    coneSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(-0.7, 3, 2)));
    pCoat coneCoat(new GlossyPaint(glm::dvec3(0.20, 0.20, 0.80), 0.7));
    std::shared_ptr<Prop> coneProp = play().propTeam3D()->createProp();
    coneProp->setSurface(coneSurf);
    coneSurf->setCoating(coneCoat);
    _props.push_back(coneProp);
}

