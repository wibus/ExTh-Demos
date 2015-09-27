#include "CpuRaytracingCharacter.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>
#include <CellarWorkbench/Misc/StringUtils.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>
#include <PropRoom2D/Team/AbstractTeam.h>

#include <PropRoom3D/Prop/Prop.h>
#include <PropRoom3D/Prop/Surface/Box.h>
#include <PropRoom3D/Prop/Surface/Sphere.h>
#include <PropRoom3D/Prop/Surface/Plane.h>
#include <PropRoom3D/Prop/Surface/Quadric.h>
#include <PropRoom3D/Prop/Material/Fog.h>
#include <PropRoom3D/Prop/Material/Glass.h>
#include <PropRoom3D/Prop/Material/Metal.h>
#include <PropRoom3D/Prop/Coating/FlatPaint.h>
#include <PropRoom3D/Prop/Coating/GlossyPaint.h>
#include <PropRoom3D/Prop/Coating/TexturedFlatPaint.h>
#include <PropRoom3D/Prop/Coating/TexturedGlossyPaint.h>
#include <PropRoom3D/Prop/Environment/Environment.h>
#include <PropRoom3D/Prop/Environment/Backdrop/ProceduralSun.h>
#include <PropRoom3D/StageSet/StageSet.h>
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


typedef std::shared_ptr<Surface> pSurf;
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

    //* Choose and setup stageSet
    setupStageStageSet();
    //setupManufacturingStageSet();
    //setupConvergenceStageSet();
    //setupQuadricStageSet();

    play().propTeam3D()->saveScene("StageSet.prop3");
    /*/

    glm::dvec3 focusPos = glm::dvec3(-1.2, -1.2, 5.25);
    glm::dvec3 camPos = focusPos + glm::dvec3(25, -40, 14) * 2.1;
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);

    play().propTeam3D()->loadScene("StageSet.prop3");
    //*/

    play().propTeam3D()->saveScene("StageSetCopy.prop3");
}

void CpuRaytracingCharacter::beginStep(const StageTime &time)
{
    float elapsedtime = time.elapsedTime();
    _ups->setText("UPS: " + toString(floor(1.0 / elapsedtime)));

    float velocity = 3.0f * elapsedtime;
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

    /*
    if(_sphere)
    {
        _sphere->surface()->transform(Transform(1.0, glm::dquat(),
            glm::dvec3(glm::sin(time.totalTime()) * time.elapsedTime(), 0.0, 0.0)));
    }
    //*/
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
    play().propTeam3D()->clearProps();
    _sphere.reset();
}

void CpuRaytracingCharacter::setupStageStageSet()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 1.0);
    glm::dvec3 camPos = focusPos + glm::dvec3(8, -15, 5) * 1.3;
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);

    // Environment
    auto env = play().propTeam3D()->stageSet()->environment();
    env->setBackdrop(std::shared_ptr<Backdrop>(new ProceduralSun(true)));
//    env->setAmbientMaterial(pMat(new Fog(glm::dvec3(0.95, 0.95, 0.95), 0.005, 40.0)));

    // Socle
    glm::dvec3 negLim(-10.0, -10.0, -0.2);
    glm::dvec3 posLim( 10.0,  10.0, 0.0);
    glm::dvec3 socleDia = posLim - negLim;
    pSurf xNeg = Plane::plane(glm::dvec3(-1, 0, 0), negLim);
    pSurf yPos = Plane::plane(glm::dvec3( 0, 1, 0), posLim);
    pSurf zNeg = Plane::plane(glm::dvec3( 0, 0,-1), negLim);
    pSurf xPos = Plane::plane(glm::dvec3( 1, 0, 0), posLim);
    pSurf yNeg = Plane::plane(glm::dvec3(0, -1, 0), negLim);
    pSurf socleSide = xNeg & xPos & yNeg & yPos & zNeg;
    pSurf zSoc = PlaneTexture::plane(glm::dvec3(0, 0, 1), posLim,
               glm::dvec3(socleDia.x/7.0, 0.0, 0.0),
               glm::dvec3(0.0, socleDia.y/7.0, 0.0),
               negLim);

    pCoat socleSideCoat(new FlatPaint(
            glm::dvec3(0.5, 0.5, 0.5)));
    socleSide->setCoating(socleSideCoat);

    pCoat socleTopCoat(new TexturedGlossyPaint(
            ":/CpuRaytracing/Bathroom_Tiles_albedo.png",
            ":/CpuRaytracing/Bathroom_Tiles_gloss.png",
             ESamplerFilter::LINEAR,
             ESamplerWrapper::REPEAT));
    zSoc->setCoating(socleTopCoat);

    std::shared_ptr<Prop> socle = play().propTeam3D()->createProp();
    socle->setSurface(zSoc & socleSide);


    // Stage
    glm::dvec3 boxMin(-5, -5, 0.0);
    glm::dvec3 boxMax( 5,  5, 5.0);
    glm::dvec3 boxDia = boxMax - boxMin;
    glm::dvec3 boxCenter = boxMin + boxDia / 2.0;
    glm::dvec3 wallThickness = glm::dvec3(0.1);
    glm::dvec3 pillard(boxMax.x, boxMin.y, boxMin.z);
    glm::dvec3 doorDim(0.85, 0.85, 2.0 * 2);

    pSurf xBot = Plane::plane(glm::dvec3(-1,  0,  0), boxMin);
    pSurf xTop = Plane::plane(glm::dvec3( 1,  0,  0), boxMax);
    pSurf yTop = Plane::plane(glm::dvec3( 0,  1,  0), boxMax);
    pSurf zTop = Plane::plane(glm::dvec3( 0,  0,  1), boxMax);
    pSurf yBot = PlaneTexture::plane(
        glm::dvec3( 0, -1,  0), boxMin,
        glm::dvec3(boxDia.x/2.0, 0, 0),
        glm::dvec3(0, 0, boxDia.z),
        boxMin);
    pSurf box = (xBot & yBot & xTop & yTop & zTop) & ~(!zSoc);
    pSurf sideWall = Plane::plane(glm::dvec3(1, 0, 0), boxCenter);
    pSurf ceiling = Plane::plane(glm::dvec3( 0, 0, -1), boxMax - wallThickness);
    pSurf pillarX = Plane::plane(glm::dvec3(-1, 0, 0), pillard - wallThickness);
    pSurf pillarY = Plane::plane(glm::dvec3( 0, 1, 0), pillard + wallThickness);
    pSurf zStep = Plane::plane(glm::dvec3(0, 0, 1), boxCenter);
    pSurf yStep = Plane::plane(glm::dvec3( 0, -1, 0), boxCenter);
    pSurf rearWall = Plane::plane(glm::dvec3( 0, -1, 0), boxMax - wallThickness);

    pSurf hallZTop = Plane::plane(glm::dvec3(0, 0, -1), boxCenter - wallThickness);
    pSurf hallXbot = Plane::plane(glm::dvec3(1, 0, 0), boxCenter + wallThickness);
    pSurf hallXtop = Plane::plane(glm::dvec3(-1, 0, 0), boxMax - wallThickness);
    pSurf hallYbot = Plane::plane(glm::dvec3(0, 1, 0), boxCenter + wallThickness);
    pSurf hallYtop = Plane::plane(glm::dvec3(0, -1, 0), boxMax - wallThickness);
    pSurf hall = hallZTop | hallXbot | hallXtop | hallYbot | hallYtop;

    pSurf roomZTop = Plane::plane(glm::dvec3(0, 0, -1), boxMax - wallThickness);
    pSurf roomXbot = Plane::plane(glm::dvec3(1, 0, 0), boxMin + wallThickness);
    pSurf roomXtop = Plane::plane(glm::dvec3(-1, 0, 0), boxCenter - wallThickness);
    pSurf roomYbot = Plane::plane(glm::dvec3(0, 1, 0), boxMin + wallThickness);
    pSurf roomYtop = Plane::plane(glm::dvec3(0, -1, 0), boxMax - wallThickness);
    pSurf room = roomZTop | roomXbot | roomXtop | roomYbot | roomYtop;


    pSurf hallEntrance = !Box::boxPosDims(glm::dvec3(boxMax.x, boxMax.y/2.0, 0.0), doorDim);
    pSurf roomEntrance = !Box::boxPosDims(glm::dvec3(boxMin.y, boxMin.y/2.0, 0.0), doorDim);
    pSurf glassPassage = !Box::boxPosDims(glm::dvec3(boxMax.x/2.0, 0.0, 0.0), doorDim);
    pSurf roomPassage = !Box::boxPosDims(glm::dvec3(0.0, boxMax.y/2.0, 0.0), doorDim);


    pSurf tallWindowHole= !Box::boxPosDims(
            glm::dvec3(0, boxMin.y * 3.0 / 4.0, boxDia.z / 2.0),
            glm::dvec3(1, boxDia.y / 4.0 - 0.50, boxDia.z - 0.50));

    pSurf longWindowHole = !Box::boxPosDims(
            glm::dvec3(boxMin.y, 0, boxDia.z * 0.75),
            glm::dvec3(1, boxDia.x * 0.9, boxDia.z * 0.20));

    pSurf smallWindowHole1 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/7.0));
    pSurf smallWindowHole2 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*3.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/7.0));
    pSurf smallWindowHole3 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*5.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/7.0));
    pSurf smallWindowHole4 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*7.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/7.0));

    pSurf stageSurf = box & hall & room &
        hallEntrance & roomEntrance & glassPassage & roomPassage &
        tallWindowHole & smallWindowHole1 & smallWindowHole2 &
        longWindowHole & smallWindowHole3 & smallWindowHole4 &
        ((rearWall | ceiling | sideWall) |
         (pillarX & pillarY) |
         (yStep & zStep));

    pCoat posterCoat(new TexturedFlatPaint(
            ":/CpuRaytracing/Fusion_Albums.png",
            ESamplerFilter::LINEAR,
            ESamplerWrapper::CLAMP,
            glm::dvec3(0.7, 0.7, 0.7)));

    std::shared_ptr<Prop> stage = play().propTeam3D()->createProp();
    stageSurf->setCoating(posterCoat);
    stage->setSurface(stageSurf);


    // Bus shelter
    glm::dmat4 sideRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(0, 0, 1));

    glm::dmat4 frontGlassTrans = glm::translate(glm::dmat4(),
        glm::dvec3((boxMax.x - wallThickness.x) / 2.0,
                   boxMin.y + wallThickness.y / 2.0,
                   0));
    createBusGlass(frontGlassTrans,
                   boxMax.x - wallThickness.x / 2.0,
                   boxMax.z - wallThickness.z,
                   wallThickness.y);

    glm::dmat4 sideFullGlassTrans = glm::translate(glm::dmat4(),
       glm::dvec3(boxMax.x - wallThickness.x / 2.0,
                  (boxMin.y + wallThickness.y) / 2.0,
                  0));
    createBusGlass(sideFullGlassTrans * sideRot,
                   boxMax.y - wallThickness.y / 2.0,
                   boxMax.z - wallThickness.z,
                   wallThickness.x);

    glm::dmat4 sideHalfGlassTrans = glm::translate(glm::dmat4(),
       glm::dvec3(boxMax.x - wallThickness.x / 2.0,
                  (boxMax.y - wallThickness.y) / 2.0,
                  (boxMax.z - boxMin.z) / 2.0));
    createBusGlass(sideHalfGlassTrans * sideRot,
                   boxMax.y - wallThickness.y / 2.0,
                   boxMax.z/2.0 - wallThickness.z,
                   wallThickness.x);


    // Fence
    double fenceSide = 0.20;
    double fenceHeight = 1.0;
    double fenceCapSide = fenceSide * 1.33;
    double fenceCapRadius = fenceCapSide * 1.05;
    double fenceWallWidth = fenceSide * 0.6;
    double fenceWallHeight = fenceHeight * 0.8;
    glm::dmat4 capXRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(1, 0, 0));
    glm::dmat4 capYRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(0, 1, 0));
    pSurf capXneg = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    capXneg->transform(glm::translate(capXRot, glm::dvec3(-fenceCapSide, 0, 0)));
    pSurf capXpos = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    capXpos->transform(glm::translate(capXRot, glm::dvec3( fenceCapSide, 0, 0)));
    pSurf capYneg = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    capYneg->transform(glm::translate(capYRot, glm::dvec3(0, -fenceCapSide, 0)));
    pSurf capYpos = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    capYpos->transform(glm::translate(capYRot, glm::dvec3(0,  fenceCapSide, 0)));
    pSurf capBox = Box::boxPosDims(glm::dvec3(0, 0, -fenceCapSide/2.0), glm::dvec3(fenceCapSide));
    pSurf capSphere = Sphere::sphere(glm::dvec3(0, 0, -fenceSide/2.0), fenceSide / 2.0);
    pSurf capSurf = capSphere | (capBox & !(capXneg | capXpos | capYneg | capYpos));
    capSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(0, 0.0, fenceHeight + fenceCapSide)));

    pSurf fencePostBeam = Box::boxPosDims(glm::dvec3(0, 0, fenceHeight/2.0), glm::dvec3(fenceSide, fenceSide, fenceHeight));
    pSurf fencePost = fencePostBeam | capSurf;

    std::vector<glm::dvec3> postPos = {
        glm::dvec3(-1.0,     -1.0,     0.0),
        glm::dvec3(-1.0/3.0, -1.0,     0.0),
        glm::dvec3( 1.0/3.0, -1.0,     0.0),
        glm::dvec3( 1.0,     -1.0,     0.0),
        glm::dvec3( 1.0,     -1.0/3.0, 0.0),
        glm::dvec3( 1.0,      1.0/3.0, 0.0),
        glm::dvec3( 1.0,      1.0,     0.0),
        glm::dvec3( 1.0/3.0,  1.0,     0.0),
        glm::dvec3(-1.0/3.0,  1.0,     0.0),
        glm::dvec3(-1.0,      1.0,     0.0),
        glm::dvec3(-1.0,      1.0/3.0, 0.0),
        glm::dvec3(-1.0,     -1.0/3.0, 0.0)
    };

    glm::dvec3 sceneDim((socleDia.x-fenceCapSide) / 2.0,
                        (socleDia.y-fenceCapSide) / 2.0,
                        0.0);
    for(const auto& pos : postPos)
    {
        glm::dvec3 postOffset = sceneDim * pos;
        glm::dmat4 postTrans = glm::translate(glm::dmat4(), postOffset);

        pSurf postShell = Shell(fencePost);
        postShell->transform(postTrans);

        pSurf postBounds = Box::boxPosDims(
            postOffset + glm::dvec3(0, 0, (fenceHeight + fenceCapSide)/2.0),
            glm::dvec3(fenceCapSide, fenceCapSide, fenceHeight + fenceCapSide));

        std::shared_ptr<Prop> postProp = play().propTeam3D()->createProp();
        postProp->setBoundingSurface(postBounds);
        postProp->setSurface(postShell);
    }

    std::shared_ptr<Prop> fenceWallPropXNeg = play().propTeam3D()->createProp();
    fenceWallPropXNeg->setSurface(Box::boxPosDims(
        glm::dvec3(-sceneDim.x, 0, fenceWallHeight/2.0),
        glm::dvec3(fenceWallWidth, 2*sceneDim.y, fenceWallHeight)));
    std::shared_ptr<Prop> fenceWallPropXPos = play().propTeam3D()->createProp();
    fenceWallPropXPos->setSurface(Box::boxPosDims(
        glm::dvec3(sceneDim.x, 0, fenceWallHeight/2.0),
        glm::dvec3(fenceWallWidth, 2*sceneDim.y, fenceWallHeight)));
    std::shared_ptr<Prop> fenceWallPropYNeg = play().propTeam3D()->createProp();
    fenceWallPropYNeg->setSurface(Box::boxPosDims(
        glm::dvec3(0, -sceneDim.y, fenceWallHeight/2.0),
        glm::dvec3(2*sceneDim.x, fenceWallWidth, fenceWallHeight)));
    std::shared_ptr<Prop> fenceWallPropYPos = play().propTeam3D()->createProp();
    fenceWallPropYPos->setSurface(Box::boxPosDims(
        glm::dvec3(0, sceneDim.y, fenceWallHeight/2.0),
        glm::dvec3(2*sceneDim.x, fenceWallWidth, fenceWallHeight)));


    // Egg (ellipsoid)
    double eggRadius = 0.3;
    double eggStandSide = eggRadius * 4;
    double eggStandHeight = eggStandSide * 0.15;
    pSurf eggTop = Quadric::ellipsoid(eggRadius, eggRadius, 2*eggRadius);
    eggTop->transform(glm::translate(glm::dmat4(), glm::dvec3(0, 0, 0)));
    pSurf eggBottom = Sphere::sphere(glm::dvec3(), eggRadius);
    pSurf topCap = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    pSurf bottomCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf eggSurf = (eggTop & ~topCap) | (eggBottom & ~bottomCap);
    eggSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(
         sceneDim.x / 4.0, -sceneDim.y / 4.0, eggRadius + eggStandHeight)));


    pMat eggMat(new Metal(glm::dvec3(1, 0.765557, 0.336057), 1.0));
    std::shared_ptr<Prop> eggProp = play().propTeam3D()->createProp();
    eggProp->setSurface(eggSurf);
    eggProp->setMaterial(eggMat);

    // Egg stand
    pSurf eggStandSurf = Box::boxPosDims(
        glm::dvec3(sceneDim.x/4, -sceneDim.y/4, eggStandHeight/2.0),
        glm::dvec3(eggStandSide, eggStandSide, eggStandHeight));
    pCoat eggStandCoat(new GlossyPaint(glm::dvec3(0.08), 1.0));
    std::shared_ptr<Prop> eggStandProp = play().propTeam3D()->createProp();
    eggStandProp->setSurface(eggStandSurf);
    eggStandSurf->setCoating(eggStandCoat);

/*
    // Bowl
    pSurf bowlBase = Sphere::sphere(glm::dvec3(0, 0, 2.7), 3.0);
    pSurf bowlSurf = bowlBase &
        !(Sphere::sphere(glm::dvec3(0, 0, 2.7), 2.6) &
          Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.5)))&
        Plane::plane(glm::dvec3(0,0,1), glm::dvec3(0,0,2.7)) &
        Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.1));
    bowlSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(15, -5, 0)));
    pMat bowlMat(new Glass(glm::dvec3(0.95, 0.75, 0.72), 6.0));

    std::shared_ptr<Prop> bowl = play().propTeam3D()->createProp();
    bowl->setBoundingSurface(bowlBase);
    bowl->setSurface(bowlSurf);
    bowl->setMaterial(bowlMat);
*/
}


void CpuRaytracingCharacter::setupManufacturingStageSet()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 0);
    glm::dvec3 camPos = glm::dvec3(0, 2, 0);
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);

    // Setup
    double glassWidth = 1.0;
    double glassDepth = 0.1;
    double glassHeight = 1.0;
    pSurf glassNegX = Plane::plane(glm::dvec3(-1, 0, 0), glm::dvec3(-glassWidth/2.0, 0, 0));
    pSurf glassPosX = Plane::plane(glm::dvec3( 1, 0, 0), glm::dvec3( glassWidth/2.0, 0, 0));
    pSurf glassNegY = Plane::plane(glm::dvec3(0, -1, 0), glm::dvec3(0, -glassDepth/2.0, 0));
    pSurf glassPosY = Plane::plane(glm::dvec3(0,  1, 0), glm::dvec3(0,  glassDepth/2.0, 0));
    pSurf glassNegZ = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    pSurf glassPosZ = Plane::plane(glm::dvec3(0, 0,  1), glm::dvec3(0, 0, glassHeight));
    pSurf glassSurf = glassNegX & glassPosX & glassNegY & glassPosY & glassPosZ & glassNegZ;

    pMat glassMat(new Glass(glm::dvec3(1.0), 0.0));

    std::shared_ptr<Prop> glassProp = play().propTeam3D()->createProp();
    glassProp->setSurface(glassSurf);
    glassProp->setMaterial(glassMat);

    pSurf ballSurf = Sphere::sphere(glm::dvec3(1.0, 1.0, 0.5), 0.5);
    std::shared_ptr<Prop> ballProp = play().propTeam3D()->createProp();
    ballProp->setSurface(ballSurf);

    pSurf floorSurf = Plane::plane(glm::dvec3(0.0, 0.0, 1.0), glm::dvec3());
    std::shared_ptr<Prop> floorProp = play().propTeam3D()->createProp();
    floorProp->setSurface(floorSurf);
}

void CpuRaytracingCharacter::setupConvergenceStageSet()
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
}

void CpuRaytracingCharacter::setupQuadricStageSet()
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

    // Paraboloid
    pSurf paraBase = Quadric::paraboloid(0.5, 0.5);
    pSurf paraTopCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 2));
    pSurf paraSurf = paraBase & paraTopCap;
    paraSurf->transform(glm::translate(glm::dmat4(), glm::dvec3(0.1, -3, 0.5)));
    pCoat paraCoat(new GlossyPaint(glm::dvec3(0.90, 0.90, 0.30), 0.15));
    std::shared_ptr<Prop> paraProp = play().propTeam3D()->createProp();
    paraProp->setSurface(paraSurf);
    paraSurf->setCoating(paraCoat);

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
}


void CpuRaytracingCharacter::createBusGlass(
        const glm::dmat4& transform,
        double width,
        double height,
        double depth)
{
    double fixturePadding = depth;
    double sidePadding = depth * 0.75;


    pSurf glassNegX = Plane::plane(glm::dvec3(-1, 0, 0), glm::dvec3(-width/2.0+sidePadding, 0, 0));
    pSurf glassPosX = Plane::plane(glm::dvec3( 1, 0, 0), glm::dvec3( width/2.0-sidePadding, 0, 0));
    pSurf glassNegY = Plane::plane(glm::dvec3(0, -1, 0), glm::dvec3(0, -0.8*depth/(2.0), 0));
    pSurf glassPosY = Plane::plane(glm::dvec3(0,  1, 0), glm::dvec3(0,  0.8*depth/(2.0), 0));
    pSurf glassNegZ = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, fixturePadding));
    pSurf glassPosZ = Plane::plane(glm::dvec3(0, 0,  1), glm::dvec3(0, 0, height - fixturePadding));
    pSurf glassSurf = glassNegX & glassPosX & glassNegY & glassPosY & glassPosZ & glassNegZ;

    pMat glassMat(new Glass(glm::dvec3(0.5, 0.5, 0.45), 4.0));

    std::shared_ptr<Prop> glassProp = play().propTeam3D()->createProp();
    glassProp->setSurface(glassSurf);
    glassProp->setMaterial(glassMat);
    glassProp->surface()->transform(transform);



    std::shared_ptr<Prop> bottomLeftFix = createFixture(fixturePadding);
    bottomLeftFix->surface()->transform(transform * glm::translate(glm::dmat4(), glm::dvec3(-width*0.30, 0, 0)));

    std::shared_ptr<Prop> bottomRightFix = createFixture(fixturePadding);
    bottomRightFix->surface()->transform(transform * glm::translate(glm::dmat4(), glm::dvec3(width*0.30, 0, 0)));


    std::shared_ptr<Prop> topLeftFix = createFixture(fixturePadding);
    topLeftFix->surface()->transform(transform * glm::translate(glm::dmat4(), glm::dvec3(-width*0.30, 0, height))
                                     *glm::rotate(glm::dmat4(), glm::pi<double>(), glm::dvec3(0, 1, 0)));

    std::shared_ptr<Prop> topRightFix = createFixture(fixturePadding);
    topRightFix->surface()->transform(transform * glm::translate(glm::dmat4(), glm::dvec3(width*0.30, 0, height))
                                      *glm::rotate(glm::dmat4(), glm::pi<double>(), glm::dvec3(0, 1, 0)));
}

std::shared_ptr<prop3::Prop> CpuRaytracingCharacter::createFixture(
        double size)
{
    double footWidth  = size * 2.0;
    double footDepth  = size * 1.0;
    double footHeight = size * 0.20;

    double legRadius = size * 0.20;
    double legHeight = size * 0.60;

    double handWidth  = size * 2.0;
    double handDepth  = size * 1.0;
    double handHeight = size * 1.0;

    double handHoleDepth = size * 0.80;
    double handHoleHeight = size * 0.80;


    pSurf footNegX = Plane::plane(glm::dvec3(-1, 0, 0), glm::dvec3(-footWidth/2.0, 0, 0));
    pSurf footPosX = Plane::plane(glm::dvec3( 1, 0, 0), glm::dvec3( footWidth/2.0, 0, 0));
    pSurf footNegY = Plane::plane(glm::dvec3(0, -1, 0), glm::dvec3(0, -footDepth/2.0, 0));
    pSurf footPosY = Plane::plane(glm::dvec3(0,  1, 0), glm::dvec3(0,  footDepth/2.0, 0));
    pSurf footNegZ = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    pSurf footPosZ = Plane::plane(glm::dvec3(0, 0,  1), glm::dvec3(0, 0, footHeight));
    pSurf footSurf = footNegX & footPosX & footNegY & footPosY & footPosZ & ~footNegZ;


    pSurf handNegX = Plane::plane(glm::dvec3(-1, 0, 0), glm::dvec3(-handWidth/2.0, 0, 0));
    pSurf handPosX = Plane::plane(glm::dvec3( 1, 0, 0), glm::dvec3( handWidth/2.0, 0, 0));
    pSurf handNegY = Plane::plane(glm::dvec3(0, -1, 0), glm::dvec3(0, -handDepth/2.0, 0));
    pSurf handPosY = Plane::plane(glm::dvec3(0,  1, 0), glm::dvec3(0,  handDepth/2.0, 0));
    pSurf handNegZ = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, footHeight + legHeight));
    pSurf handPosZ = Plane::plane(glm::dvec3(0, 0,  1), glm::dvec3(0, 0, footHeight + legHeight + handHeight));
    pSurf holeNegY = Plane::plane(glm::dvec3(0, -1, 0), glm::dvec3(0, -handHoleDepth/2.0, 0.0));
    pSurf holePosY = Plane::plane(glm::dvec3(0,  1, 0), glm::dvec3(0, handHoleDepth/2.0, 0.0));
    pSurf holeNegZ = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, footHeight + legHeight + handHeight - handHoleHeight));
    pSurf handSurf = handNegX & handPosX & handNegY & handPosY & handPosZ & handNegZ &
                     !(holeNegY & holePosY & holeNegZ);

    pSurf legCylinder(Quadric::cylinder(legRadius, legRadius));
    pSurf legPosZ = Plane::plane(glm::dvec3(0, 0,  1), glm::dvec3(0, 0, footHeight));
    pSurf legNegZ = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, footHeight + legHeight));
    pSurf legSurf = legCylinder & ~(!legPosZ) & ~(!legNegZ);

    pSurf bBoxNegX = footNegX & footPosX & footNegY & footPosY & footNegZ & handPosZ;

    pMat fixtureMat(new Metal(glm::dvec3(0.4, 0.4, 0.4), 0.85));

    pSurf fixtureSurf = footSurf | handSurf | legSurf;
    std::shared_ptr<Prop> fixtureProp = play().propTeam3D()->createProp();
    fixtureProp->setBoundingSurface(bBoxNegX);
    fixtureProp->setSurface(fixtureSurf);
    fixtureProp->setMaterial(fixtureMat);

    return fixtureProp;
}
