#include "CpuRaytracingCharacter.h"

#include <iostream>

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>
#include <CellarWorkbench/Misc/StringUtils.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>
#include <PropRoom2D/Team/AbstractTeam.h>

#include <PropRoom3D/Node/StageSet.h>
#include <PropRoom3D/Prop/Prop.h>
#include <PropRoom3D/Prop/Surface/Box.h>
#include <PropRoom3D/Prop/Surface/Sphere.h>
#include <PropRoom3D/Prop/Surface/Plane.h>
#include <PropRoom3D/Prop/Surface/Quadric.h>
#include <PropRoom3D/Prop/Material/UniformStdMaterial.h>
#include <PropRoom3D/Prop/Coating/UniformStdCoating.h>
#include <PropRoom3D/Prop/Coating/TexturedStdCoating.h>
#include <PropRoom3D/Light/Environment.h>
#include <PropRoom3D/Light/Backdrop/ProceduralSun.h>
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


typedef std::shared_ptr<StageZone> pZone;
typedef std::shared_ptr<Surface> pSurf;
typedef std::shared_ptr<Material> pMat;
typedef std::shared_ptr<Coating> pCoat;
typedef std::shared_ptr<Prop> pProp;


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

    play().propTeam3D()->saveScene("StageSet.prop3");
}

void CpuRaytracingCharacter::beginStep(const StageTime &time)
{
    float elapsedtime = time.elapsedTime();
    _ups->setText("UPS: " + toString(floor(1.0 / elapsedtime)));

    float velocity = 4.0f * elapsedtime;
    const float turnSpeed = 0.004f;

    SynchronousMouse& syncMouse = *play().synchronousMouse();
    SynchronousKeyboard& syncKeyboard = *play().synchronousKeyboard();

    bool moved = false;

    if(syncKeyboard.isAsciiPressed('w'))
    {
        _camMan->forward(velocity);
        moved = true;
    }
    if(syncKeyboard.isAsciiPressed('s'))
    {
        _camMan->forward(-velocity);
        moved = true;
    }
    if(syncKeyboard.isAsciiPressed('a'))
    {
        _camMan->sideward(-velocity);
        moved = true;
    }
    if(syncKeyboard.isAsciiPressed('d'))
    {
        _camMan->sideward(velocity);
        moved = true;
    }

    if(syncMouse.displacement() != glm::ivec2(0, 0) &&
       syncMouse.buttonIsPressed(EMouseButton::LEFT))
    {
        _camMan->pan( syncMouse.displacement().x * -turnSpeed);
        _camMan->tilt(syncMouse.displacement().y * -turnSpeed);
    }

    if(moved)
    {
        glm::dvec3 p = _camMan->position();
        std::cout << "Pos: (" << p.x << ", " << p.y << ", " << p.z << "); ";
        std::cout << "Dist: " << glm::distance(p, glm::dvec3()) << "m" << std::endl;
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
    play().propTeam3D()->reset();
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

    // Bounding Hierarchy
    std::shared_ptr<StageZone> stageSet = play().propTeam3D()->stageSet();



    ///////////
    // Socle //
    ///////////
    glm::dvec3 socelMin(-10.0, -10.0, -0.2);
    glm::dvec3 socelMax( 10.0,  10.0, 0.0);
    glm::dvec3 socleDia = socelMax - socelMin;

    pSurf soceSurf = BoxTexture::boxCorners(
            socelMin, socelMax, // Corners
            socelMin,         // Tex Origin
            glm::dvec3(socleDia.x/7.0, 0.0, 0.0),
            glm::dvec3(0.0, socleDia.y/7.0, 0.0),
            true);

    auto socleCoat = new TexturedStdCoating();
    socleCoat->setRoughnessTexName(":/CpuRaytracing/Bathroom_Tiles_gloss.png");
    socleCoat->setDefaultRoughness(0.0);
    socleCoat->setPaintColorTexName(":/CpuRaytracing/Bathroom_Tiles_albedo.png");
    socleCoat->setDefaultPaintColor(glm::dvec4(color::white, 1.0));
    socleCoat->setPaintRefractiveIndex(1.55);
    socleCoat->setTexFilter(ESamplerFilter::LINEAR);
    socleCoat->setTexWrapper(ESamplerWrapper::REPEAT);

    pProp socleProp(new Prop("Socle"));
    soceSurf->setCoating(pCoat(socleCoat));
    socleProp->addSurface(soceSurf);
    stageSet->addProp(socleProp);


    ///////////
    // Stage //
    ///////////
    glm::dvec3 boxMin(-5, -5, 0.0);
    glm::dvec3 boxMax( 5,  5, 5.0);
    glm::dvec3 boxDia = boxMax - boxMin;
    glm::dvec3 boxCenter = boxMin + boxDia / 2.0;
    glm::dvec3 wallThickness = glm::dvec3(0.1);
    glm::dvec3 doorDim(0.85, 0.85, 2.0 * 2);

    glm::dvec3 hallMin(boxCenter.x + wallThickness.x, boxCenter.y + wallThickness.y, -1.0);
    glm::dvec3 hallMax(boxMax.x - wallThickness.x, boxMax.y - wallThickness.y, boxCenter.z - wallThickness.z);
    glm::dvec3 boudMin(boxCenter.x + wallThickness.x, boxMin.y - wallThickness.y, -1.0);
    glm::dvec3 boudMax(boxMax.x + wallThickness.x, boxCenter.y - wallThickness.y, boxMax.z - wallThickness.z);
    glm::dvec3 roomMin(boxMin.x + wallThickness.x, boxMin.y + wallThickness.y, -1.0);
    glm::dvec3 roomMax(boxCenter.x - wallThickness.x, boxMax.y - wallThickness.y, boxMax.z - wallThickness.z);

    pSurf box = Box::boxCorners( glm::dvec3(boxMin.x, boxMin.y, socelMin.z), boxMax);

    pSurf hall = !Box::boxCorners(hallMin, hallMax);
    pSurf room = !Box::boxCorners(roomMin, roomMax);
    pSurf boud = !Box::boxCorners(boudMin, boudMax);

    pSurf hallEntrance = !Box::boxPosDims(glm::dvec3(boxMax.x, boxMax.y/2.0, 0.0), doorDim);
    pSurf roomEntrance = !Box::boxPosDims(glm::dvec3(boxMin.y, boxMin.y/2.0, 0.0), doorDim);
    pSurf glassPassage = !Box::boxPosDims(glm::dvec3(boxMax.x/2.0, 0.0, 0.0), doorDim);
    pSurf roomPassage = !Box::boxPosDims(glm::dvec3(0.0, boxMax.y/2.0, 0.0), doorDim);

    pSurf crossWindowHole= !(
        Box::boxPosDims(
            glm::dvec3(0, boxMin.y/2.0, 2),
            glm::dvec3(wallThickness.x * 3.0, 1.0, 3.0)) |
        Box::boxPosDims(
            glm::dvec3(0, boxMin.y/2.0, 3.0),
            glm::dvec3(wallThickness.x * 3.0, 3.0, 1.0)));

    pSurf longWindowHole = !Box::boxPosDims(
            glm::dvec3(boxMin.y, 0, boxDia.z * 0.75),
            glm::dvec3(1, boxDia.y * 0.8, boxDia.z * 0.20));

    pSurf smallWindowHole1 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole2 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*3.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole3 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*5.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole4 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*7.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole5 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*5.0/8.0, boxMin.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole6 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*7.0/8.0, boxMin.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));

    pSurf stageSurf = box & hall & room & boud &
        hallEntrance & roomEntrance & glassPassage & roomPassage &
        smallWindowHole1 & smallWindowHole2 & smallWindowHole3 &
        smallWindowHole4 & smallWindowHole5 & smallWindowHole6 &
        crossWindowHole & longWindowHole;

    pSurf stripWallSurf = createHoleStrippedWall(
        glm::dvec3(boxDia.x / 2.0, wallThickness.y, boxDia.z),
        0.35, 0.35, 0.35);

    pSurf ynegStripWall = Surface::shell(stripWallSurf);
    Surface::translate(ynegStripWall, glm::dvec3(boxMax.x/2.0, boxMin.y+wallThickness.y/2.0, 0));
    pSurf xposStripWall = Surface::shell(stripWallSurf);
    Surface::rotate(xposStripWall, -glm::pi<double>()/2.0, glm::dvec3(0, 0, 1));
    Surface::translate(xposStripWall, glm::dvec3(boxMax.x-wallThickness.x/2.0, boxMin.y/2.0, 0));

    pCoat stageCoat = coating::createClearCoat(1.0);
    stageSurf->setCoating(stageCoat);
    xposStripWall->setCoating(stageCoat);
    ynegStripWall->setCoating(stageCoat);


    pProp stageProp(new Prop("Stage"));
    stageProp->addSurface(stageSurf);
    pZone stageZone(new StageZone("Stage Zone"));
    stageSet->addSubzone(stageZone);
    stageZone->setBounds( Box::boxCorners(boxMin, boxMax) );
    stageZone->addProp(stageProp);

    pProp xStrippedProp(new Prop("X Stripped Wall"));
    xStrippedProp->addSurface(xposStripWall);
    pZone xStrippedZone(new StageZone("X Stripped Zone"));
    stageZone->addSubzone(xStrippedZone);
    xStrippedZone->setBounds( Box::boxCorners(
        glm::dvec3(boxMax.x - wallThickness.x, boxMin.y, boxMin.z),
        glm::dvec3(boxMax.x, 0.0, boxMax.z)));
    xStrippedZone->addProp(xStrippedProp);

    pProp yStrippedProp(new Prop("Y Stripped Wall"));
    yStrippedProp->addSurface(ynegStripWall);
    pZone yStrippedZone(new StageZone("Y Stripped Zone"));
    stageZone->addSubzone(yStrippedZone);
    yStrippedZone->setBounds( Box::boxCorners(
        glm::dvec3(0.0, boxMin.y, boxMin.z),
        glm::dvec3(boxMax.x, boxMin.y + wallThickness.y, boxMax.z)));
    yStrippedZone->addProp(yStrippedProp);


    //////////
    // Roof //
    //////////
    double roofSide = 0.2;
    glm::dvec3 roofMin(boxMin.x * 1.80, boxMin.y * 1.80, boxMax.z);
    glm::dvec3 roofMax(boxMax.x * 1.30, boxMax.y * 1.80, boxMax.z + roofSide);
    pSurf roofSurf = Box::boxCorners(roofMin, roofMax);

    double roofPillarSide = wallThickness.x * 2.0;
    double pillarBootSide = roofPillarSide * 1.5;
    glm::dvec2 pillarXnYn(roofMin.x+pillarBootSide, roofMin.y+pillarBootSide);
    glm::dvec2 pillarXpYn(roofMax.x-pillarBootSide, roofMin.y+pillarBootSide);
    glm::dvec2 pillarXnYp(roofMin.x+pillarBootSide, roofMax.y-pillarBootSide);
    glm::dvec2 pillarXpYp(roofMax.x-pillarBootSide, roofMax.y-pillarBootSide);
    pSurf roofPillardXnYn = Box::boxPosDims(
        glm::dvec3(pillarXnYn, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));
    pSurf roofPillardXpYn = Box::boxPosDims(
        glm::dvec3(pillarXpYn, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));
    pSurf roofPillardXnYp = Box::boxPosDims(
        glm::dvec3(pillarXnYp, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));
    pSurf roofPillardXpYp = Box::boxPosDims(
        glm::dvec3(pillarXpYp, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));

    pSurf pillarBootXnYn = Box::boxPosDims(
        glm::dvec3(pillarXnYn, pillarBootSide/2.0), glm::dvec3(pillarBootSide));
    pSurf pillarBootXpYn = Box::boxPosDims(
        glm::dvec3(pillarXpYn, pillarBootSide/2.0), glm::dvec3(pillarBootSide));
    pSurf pillarBootXnYp = Box::boxPosDims(
        glm::dvec3(pillarXnYp, pillarBootSide/2.0), glm::dvec3(pillarBootSide));
    pSurf pillarBootXpYp = Box::boxPosDims(
        glm::dvec3(pillarXpYp, pillarBootSide/2.0), glm::dvec3(pillarBootSide));

    roofSurf = roofSurf |
        roofPillardXnYn | roofPillardXpYn | roofPillardXnYp | roofPillardXpYp |
        pillarBootXnYn  | pillarBootXpYn  | pillarBootXnYp  | pillarBootXpYp;

    pCoat roofCoat = coating::createClearCoat(0.5);
    roofSurf->setInnerMaterial(material::TITANIUM);
    roofSurf->setCoating(roofCoat);

    pProp roofProp(new Prop("Roof"));
    roofProp->addSurface(roofSurf);
    stageSet->addProp(roofProp);


    ////////////////////////
    // Ceiling decoration //
    ////////////////////////
    double ceilThickness = 0.2;
    glm::dvec3 ceilCenter(boxMax.x/2.0, boxMin.y/2.0, boxMax.z - wallThickness.z - ceilThickness/2.0);
    glm::dvec3 ceilDims((boxMax.x - wallThickness.x)*0.8, (boxMax.y - wallThickness.y)*0.8, ceilThickness);
    pSurf ceilBase = Box::boxPosDims(ceilCenter, ceilDims);
    pSurf ceilHole1 = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*7/9.0, ceilDims.y*7/9.0, ceilThickness*2));
    pSurf ceilInner = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*5/9.0, ceilDims.y*5/9.0, ceilThickness*2));
    pSurf ceilHole2 = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*3/9.0, ceilDims.y*3/9.0, ceilThickness*2));
    pSurf ceilCore = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*1/9.0, ceilDims.y*1/9.0, ceilThickness*2));
    pSurf ceilSurf = ceilBase & !(ceilHole1 & !(ceilInner & !(ceilHole2 & !(ceilCore))));

    auto ceilCoat = coating::createClearCoat(0.1);
    ceilSurf->setInnerMaterial(material::SILVER);
    ceilSurf->setCoating(ceilCoat);

    pProp ceilProp(new Prop("Ceiling"));
    ceilProp->addSurface(ceilSurf);

    pZone ceilZone(new StageZone("Ceiling Zone"));
    stageZone->addSubzone(ceilZone);
    ceilZone->setBounds( ceilBase );
    ceilZone->addProp(ceilProp);


    /////////////
    // Posters //
    /////////////
    double posterLength = boxDia.z / 2 - wallThickness.z * 2.0;
    double posterEpsilon = 0.002;

    pSurf herbieSextantSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength, posterEpsilon, posterLength),
        glm::dvec3(posterLength/2.0, 0, -posterLength * 3.0/2.0),
        glm::dvec3(-posterLength*2.0, 0, 0),
        glm::dvec3(0, 0, posterLength*2.0),
        true);
    Surface::translate(herbieSextantSurf, glm::dvec3(
        boxMax.x / 2.0, boxMax.y + posterEpsilon * 10.0, boxMax.z * 0.72));

    pSurf herbieCrossingsSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength, posterEpsilon, posterLength),
        glm::dvec3(posterLength * 3.0/2.0, 0, -posterLength * 3.0/2.0),
        glm::dvec3(-posterLength*2.0, 0, 0),
        glm::dvec3(0, 0, posterLength*2.0),
        true);
    Surface::translate(herbieCrossingsSurf, glm::dvec3(
        boxMin.x / 2.0,  boxMax.y + posterEpsilon * 10.0,  boxMax.z * 0.72));

    pSurf bitchesBrewSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength*2.0, posterEpsilon, posterLength),
        glm::dvec3(-posterLength, 0, -posterLength / 2.0),
        glm::dvec3(posterLength*2.0, 0, 0),
        glm::dvec3(0, 0, posterLength*2.0),
        true);
    Surface::translate(bitchesBrewSurf, glm::dvec3(
        boxMin.x / 2.0,  boxMin.y - posterEpsilon * 10.0,  boxMax.z * 0.72));

    auto pPosterCoat = new TexturedStdCoating();
    pPosterCoat->setPaintColorTexName(":/CpuRaytracing/Fusion_Albums.png");
    pPosterCoat->setDefaultPaintColor(glm::dvec4(0.2, 0.2, 0.2, 1.0));
    pPosterCoat->setTexFilter(ESamplerFilter::LINEAR);
    pPosterCoat->setTexWrapper(ESamplerWrapper::CLAMP);
    pPosterCoat->setDefaultRoughness(1.0);
    pCoat posterCoat = pCoat(pPosterCoat);

    pProp postersProp(new Prop("Posters"));
    postersProp->addSurface(herbieSextantSurf);
    herbieSextantSurf->setCoating(posterCoat);
    postersProp->addSurface(herbieCrossingsSurf);
    herbieCrossingsSurf->setCoating(posterCoat);
    postersProp->addSurface(bitchesBrewSurf);
    bitchesBrewSurf->setCoating(posterCoat);

    stageSet->addProp(postersProp);


    ///////////
    // Fence //
    ///////////
    double fenceSide = 0.20;
    double fenceHeight = 1.0;
    double fenceCapSide = fenceSide * 1.33;
    double fenceCapRadius = fenceCapSide * 1.05;
    double fenceWallWidth = fenceSide * 0.6;
    double fenceWallHeight = fenceHeight * 0.8;
    glm::dvec3 sceneDim((socleDia.x-fenceCapSide) / 2.0,
                        (socleDia.y-fenceCapSide) / 2.0,
                        0.0);

    glm::dmat4 capXRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(1, 0, 0));
    glm::dmat4 capYRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(0, 1, 0));
    pSurf capXneg = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capXneg, glm::translate(capXRot, glm::dvec3(-fenceCapSide, 0, 0)));
    pSurf capXpos = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capXpos, glm::translate(capXRot, glm::dvec3( fenceCapSide, 0, 0)));
    pSurf capYneg = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capYneg, glm::translate(capYRot, glm::dvec3(0, -fenceCapSide, 0)));
    pSurf capYpos = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capYpos, glm::translate(capYRot, glm::dvec3(0,  fenceCapSide, 0)));
    pSurf capBox = Box::boxPosDims(glm::dvec3(0, 0, -fenceCapSide/2.0), glm::dvec3(fenceCapSide));
    pSurf capSphere = Sphere::sphere(glm::dvec3(0, 0, -fenceSide/2.0), fenceSide / 2.0);
    pSurf capSurf = capSphere | (capBox & !(capXneg | capXpos | capYneg | capYpos));
    Surface::translate(capSurf, glm::dvec3(0, 0.0, fenceHeight + fenceCapSide));

    pSurf fencePostBeam = Box::boxPosDims(glm::dvec3(0, 0, fenceHeight/2.0), glm::dvec3(fenceSide, fenceSide, fenceHeight));
    pSurf fencePost = fencePostBeam | capSurf;
    pCoat fenceCoat = stageCoat;
    fencePost->setCoating(fenceCoat);

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

    pZone fenceZone(new StageZone("Fence Zone"));
    stageSet->addSubzone(fenceZone);
    for(const auto& pos : postPos)
    {
        glm::dvec3 postOffset = sceneDim * pos;

        pSurf postShell = Surface::shell(fencePost);
        Surface::translate(postShell, postOffset);

        pZone postZone(new StageZone("Post Zone"));
        fenceZone->addSubzone(postZone);
        postZone->setBounds(Box::boxPosDims(
            postOffset + glm::dvec3(0, 0, (fenceHeight + fenceCapSide)/2.0),
            glm::dvec3(fenceCapSide, fenceCapSide, fenceHeight + fenceCapSide)));

        pProp postProp(new Prop("Fence Post"));
        postProp->addSurface(postShell);
        postZone->addProp(postProp);
    }

    pProp fenceWallsProp(new Prop("Fence Walls"));

    pSurf fenceWallSurfXNeg = Box::boxPosDims(
        glm::dvec3(-sceneDim.x, 0, fenceWallHeight/2.0),
        glm::dvec3(fenceWallWidth, 2*sceneDim.y, fenceWallHeight));
    fenceWallSurfXNeg->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfXNeg);

    pSurf fenceWallSurfXPos = Box::boxPosDims(
        glm::dvec3(sceneDim.x, 0, fenceWallHeight/2.0),
        glm::dvec3(fenceWallWidth, 2*sceneDim.y, fenceWallHeight));
    fenceWallSurfXPos->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfXPos);

    pSurf fenceWallSurfYNeg = Box::boxPosDims(
        glm::dvec3(0, -sceneDim.y, fenceWallHeight/2.0),
        glm::dvec3(2*sceneDim.x, fenceWallWidth, fenceWallHeight));
    fenceWallSurfYNeg->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfYNeg);

    pSurf fenceWallSurfYPos = Box::boxPosDims(
        glm::dvec3(0, sceneDim.y, fenceWallHeight/2.0),
        glm::dvec3(2*sceneDim.x, fenceWallWidth, fenceWallHeight));
    fenceWallSurfYPos->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfYPos);

    fenceZone->addProp(fenceWallsProp);


    /////////////////////
    // Egg (ellipsoid) //
    /////////////////////
    double eggRadius = 0.3;
    double eggStandSide = eggRadius * 4;
    double eggStandHeight = eggStandSide * 0.15;
    glm::dvec3 eggPos(sceneDim.x / 4.0, -sceneDim.y / 4.0, 0.0);

    pSurf eggTop = Quadric::ellipsoid(eggRadius, eggRadius, 2*eggRadius);
    Surface::translate(eggTop, glm::dvec3(0, 0, 0));
    pSurf eggBottom = Sphere::sphere(glm::dvec3(), eggRadius);
    pSurf topCap = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    pSurf bottomCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf eggSurf = (eggTop & ~topCap) | (eggBottom & ~bottomCap);
    Surface::translate(eggSurf, glm::dvec3(eggPos.x, eggPos.y, eggRadius + eggStandHeight));

    pSurf eggStandSurf = Box::boxPosDims(
        glm::dvec3(eggPos.x, eggPos.y, eggStandHeight/2.0),
        glm::dvec3(eggStandSide, eggStandSide, eggStandHeight));

    pZone eggZone(new StageZone("Egg Zone"));
    stageZone->addSubzone(eggZone);
    eggZone->setBounds(Box::boxCorners(
        eggPos - glm::dvec3(eggStandSide/2, eggStandSide/2, 0.0),
        eggPos + glm::dvec3(eggStandSide/2, eggStandSide/2, eggStandHeight + 3 * eggRadius)));

    eggSurf->setInnerMaterial(material::GOLD);

    pCoat eggStandCoat = coating::createClearPaint(glm::dvec3(0.08, 0.08, 0.08), 0.0);
    eggStandSurf->setCoating(eggStandCoat);

    pProp eggProp(new Prop("Egg"));
    eggProp->addSurface(eggSurf);
    eggProp->addSurface(eggStandSurf);
    eggZone->addProp(eggProp);


    pZone workZone(new StageZone("Work Zone"));
    stageZone->addSubzone(workZone);

    ////////////////
    // Work table //
    ////////////////
    double workTableTopThick = 0.04;
    double workTableLegThick = 0.10;
    glm::dvec3 workTableDims(1.0, 2.0, 0.80);
    glm::dvec2 workTablePos(boxMin.x + workTableDims.x/2.0 + wallThickness.x + 0.05,
                        boxMax.y*0.85 - workTableDims.y/2.0 - wallThickness.y - 0.05);
    glm::dvec2 workTableLegOff((workTableDims.x-workTableLegThick)/2.0,
                           (workTableDims.y-workTableLegThick)/2.0);

    pSurf workTableTop = Box::boxPosDims(
        glm::dvec3(workTablePos, workTableDims.z - workTableTopThick/2.0),
        glm::dvec3(workTableDims.x, workTableDims.y, workTableTopThick));
    pSurf workTableLeg1 = Box::boxPosDims(
        glm::dvec3(workTablePos.x-workTableLegOff.x, workTablePos.y-workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableLeg2 = Box::boxPosDims(
        glm::dvec3(workTablePos.x+workTableLegOff.x, workTablePos.y-workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableLeg3 = Box::boxPosDims(
        glm::dvec3(workTablePos.x+workTableLegOff.x, workTablePos.y+workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableLeg4 = Box::boxPosDims(
        glm::dvec3(workTablePos.x-workTableLegOff.x, workTablePos.y+workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableSurf = workTableTop | workTableLeg1 | workTableLeg2 | workTableLeg3 | workTableLeg4;

    pZone workTableZone(new StageZone("Work Table Zone"));
    workZone->addSubzone(workTableZone);
    workTableZone->setBounds( Box::boxPosDims(
        glm::dvec3(workTablePos, workTableDims.z/2.0), workTableDims));

    pProp workTableProp(new Prop("Work Table"));
    workTableProp->addSurface(workTableSurf);
    workTableZone->addProp(workTableProp);


    //////////
    // Bowl //
    //////////
    double bowlRadius = 0.18;
    pSurf bowlBase = Sphere::sphere(glm::dvec3(0, 0, bowlRadius*0.8), bowlRadius);
    pSurf bowlSurf = bowlBase &
        !(Sphere::sphere(glm::dvec3(0, 0, bowlRadius*0.8), bowlRadius*0.75) &
          Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,bowlRadius*0.2)))&
        Plane::plane(glm::dvec3(0,0,1), glm::dvec3(0,0,bowlRadius*0.8)) &
        Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.001));
    Surface::translate(bowlSurf,
        glm::dvec3(workTablePos.x, workTablePos.y - workTableDims.y / 4.0, workTableDims.z));

    pMat bowlMat = material::createInsulator(
        glm::dvec3(0.95, 0.75, 0.72),
        material::GLASS_REFRACTIVE_INDEX,
        0.975,
        0.0);
    bowlSurf->setInnerMaterial(bowlMat);

    pZone bowlZone(new StageZone("Bowl Zone"));
    workZone->addSubzone(bowlZone);
    bowlZone->setBounds(bowlBase);

    pProp bowlProp(new Prop("Bowl"));
    bowlProp->addSurface(bowlSurf);
    bowlZone->addProp(bowlProp);


    //////////
    // Lamp //
    //////////
    pSurf lampSurf;
    glm::dvec3 lampPos = glm::dvec3(workTablePos, 0.0) +
        glm::dvec3(0, workTableDims.y/4.0, workTableDims.z);

    // Head
    double headRot = glm::pi<double>() / 16;
    double headTwist = -glm::pi<double>() / 4;
    double elbowRot = glm::pi<double>() / 1.5;
    double shoulderRot = -glm::pi<double>() / 1.5;


    double headRad = 0.05;
    double headLen = 0.10;
    double headThick = 0.015;
    double neckRad = headRad * 0.5;
    double neckLen = headLen * 0.4;
    pSurf headOut = Quadric::paraboloid(1, 1);
    pSurf headIn = Quadric::paraboloid(1, 1);
    Surface::translate(headIn, glm::dvec3(0, 0, headThick));
    pSurf headCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 1));
    pSurf headSurf = headCap & headOut & !headIn;
    Surface::transform(headSurf, glm::scale(glm::dmat4(), glm::dvec3(headRad, headRad, headLen)));
    pSurf neckCyl = Quadric::cylinder(neckRad, neckRad);
    pSurf neckTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, neckLen/2.0));
    pSurf neckBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0,-neckLen/2.0));
    pSurf neckSurf = neckCyl & neckTop & neckBot;

    lampSurf = headSurf | neckSurf;
    Surface::rotate(lampSurf, glm::pi<double>(), glm::dvec3(0, 1, 0));
    Surface::rotate(lampSurf, headTwist, glm::dvec3(1, 0, 0));
    Surface::rotate(lampSurf, headRot, glm::dvec3(0, 1, 0));


    double forearmLen = 0.40;
    double forearmRad = 0.005;
    pSurf forearmCyl = Quadric::cylinder(forearmRad, forearmRad);
    pSurf forearmTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf forearmBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -forearmLen));
    pSurf foreArmSurf = forearmCyl & forearmTop & forearmBot;
    Surface::rotate(foreArmSurf, glm::pi<double>()/2.0, glm::dvec3(0, 1, 0));

    lampSurf = lampSurf | foreArmSurf;
    Surface::translate(lampSurf, glm::dvec3(forearmLen, 0, 0));


    double elbowRad = 0.02;
    double elbowWidth = 0.015;
    pSurf elbowCyl = Quadric::cylinder(elbowRad, elbowRad);
    pSurf elbowTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, elbowWidth/2.0));
    pSurf elbowBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -elbowWidth/2.0));
    pSurf elbowSurf = elbowCyl & elbowTop & elbowBot;
    Surface::rotate(elbowSurf, glm::pi<double>()/2.0, glm::dvec3(1, 0, 0));

    lampSurf = lampSurf | elbowSurf;
    Surface::rotate(lampSurf, elbowRot, glm::dvec3(0, 1, 0));


    double armLen = forearmLen;
    double armRad = forearmRad;
    pSurf armCyl = Quadric::cylinder(armRad, armRad);
    pSurf armTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf armBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -armLen));
    pSurf armSurf = armCyl & armTop & armBot;
    Surface::rotate(armSurf, glm::pi<double>()/2.0, glm::dvec3(0, 1, 0));

    lampSurf = lampSurf | armSurf;
    Surface::translate(lampSurf, glm::dvec3(armLen, 0, 0));


    double shoulderRad = elbowRad * 0.8;
    double shoulderWidth = elbowWidth * 2.0;
    pSurf shoulderCyl = Quadric::cylinder(shoulderRad, shoulderRad);
    pSurf shoulderTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, shoulderWidth/2.0));
    pSurf shoulderBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -shoulderWidth/2.0));
    pSurf shoulderSurf = shoulderCyl & shoulderTop & shoulderBot;
    Surface::rotate(shoulderSurf, glm::pi<double>()/2.0, glm::dvec3(1, 0, 0));

    lampSurf = lampSurf | shoulderSurf;
    Surface::rotate(lampSurf, shoulderRot, glm::dvec3(0, 1, 0));
    Surface::translate(lampSurf, glm::dvec3(0, 0, shoulderRad));


    double bodyRad = 0.10;
    double bodyHeight = 0.01;
    double shoulderOffset = -bodyRad * 0.65;
    double creaseHeight = bodyHeight * 0.35;
    double creaseOffset = bodyRad * 0.2;
    double creaseRad = bodyRad * 0.4;
    pSurf bodyCyl = Quadric::cylinder(bodyRad, bodyRad);
    pSurf bodyTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf bodyBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -bodyHeight));
    pSurf bodySurf = bodyCyl & bodyTop & bodyBot;
    pSurf creaseCone = Quadric::cone(creaseRad, creaseRad);
    pSurf creaseBottom = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 1.0-creaseHeight));
    pSurf creaseSurf = creaseCone & creaseBottom;
    Surface::translate(creaseSurf, glm::dvec3(creaseOffset, 0, -1.0));
    bodySurf = bodySurf & !creaseSurf;

    Surface::translate(lampSurf, glm::dvec3(shoulderOffset, 0, 0));
    lampSurf = lampSurf | bodySurf;
    Surface::translate(lampSurf, glm::dvec3(0, 0, bodyHeight));

    Surface::translate(lampSurf, lampPos);

    double lampBoundsTop = armLen*1.05;
    pZone lampZone(new StageZone("Lamp Zone"));
    workZone->addSubzone(lampZone);
    lampZone->setBounds(Box::boxPosDims(
        glm::dvec3(-0.02, 0, armLen/2.0*1.05),
        glm::dvec3(armLen*1.35, bodyRad*2.25, lampBoundsTop)));


    pCoat lampCoat = coating::createClearCoat(0.25);
    lampSurf->setCoating(lampCoat);
    lampSurf->setInnerMaterial(material::TITANIUM);

    pProp lampProp(new Prop("Lamp"));
    lampProp->addSurface(lampSurf);
    lampZone->addProp(lampProp);


    // Lamp bulb
    double lamLightRad = headRad * 0.5;
    pSurf lampBulbSurf = Sphere::sphere(glm::dvec3(0, 0, headLen/3.0), lamLightRad);
    Surface::rotate(lampBulbSurf, glm::pi<double>(), glm::dvec3(0, 1, 0));
    Surface::rotate(lampBulbSurf, headTwist, glm::dvec3(1, 0, 0));
    Surface::rotate(lampBulbSurf, headRot, glm::dvec3(0, 1, 0));
    Surface::translate(lampBulbSurf, glm::dvec3(forearmLen, 0, 0));
    Surface::rotate(lampBulbSurf, elbowRot, glm::dvec3(0, 1, 0));
    Surface::translate(lampBulbSurf, glm::dvec3(armLen, 0, 0));
    Surface::rotate(lampBulbSurf, shoulderRot, glm::dvec3(0, 1, 0));
    Surface::translate(lampBulbSurf, glm::dvec3(0, 0, shoulderRad));
    Surface::translate(lampBulbSurf, glm::dvec3(shoulderOffset, 0, 0));
    Surface::translate(lampBulbSurf, glm::dvec3(0, 0, bodyHeight));
    Surface::translate(lampBulbSurf, lampPos);
    pProp lampBulbProp(new Prop("Lamp Bulb"));
    lampBulbProp->addSurface(lampBulbSurf);
    lampZone->addProp(lampBulbProp);


    // Work zone bounds
    workZone->setBounds(Box::boxCorners(
        glm::dvec3(workTablePos.x - workTableDims.x/2, workTablePos.y - workTableDims.y/2, 0.0),
        glm::dvec3(workTablePos.x + workTableDims.x/2, workTablePos.y + workTableDims.y/2,
                   workTableDims.z + lampBoundsTop)));


    /////////////////////
    // Sculpture table //
    /////////////////////
    double scultpTableRadius = 0.30;
    double sculptTableHeight = 1.20;
    glm::dvec3 sculptTablePos(boxMin.x/2.0, boxMin.y/2.0, 0);

    double sculptTableTopThick = 0.02;
    double sculptTableTopRadius = scultpTableRadius + sculptTableTopThick;

    pSurf sculptTableTop = Quadric::paraboloid(scultpTableRadius, scultpTableRadius);
    Surface::translate(sculptTableTop, glm::dvec3(0,0, -0.25));
    pSurf sculptTableBot = Quadric::paraboloid(scultpTableRadius, scultpTableRadius);
    Surface::rotate(sculptTableBot, glm::pi<double>(), glm::dvec3(1, 0, 0));
    Surface::translate(sculptTableBot, glm::dvec3(0,0, 0.25));
    pSurf sculptTopPlane = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0.75));
    pSurf sculptBotPlane = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0,-0.75));
    pSurf sculptTableSurf = (sculptTableTop | sculptTableBot) & sculptBotPlane & sculptTopPlane;
    Surface::transform(sculptTableSurf, glm::scale(glm::dmat4(), glm::dvec3(1, 1, sculptTableHeight / 1.50)));
    Surface::translate(sculptTableSurf, glm::dvec3(0, 0, sculptTableHeight*0.5 + 0.001));
    Surface::translate(sculptTableSurf, sculptTablePos);

    sculptTableSurf->setInnerMaterial(material::GLASS);

    pSurf sculptTopCyl = Quadric::cylinder(sculptTableTopRadius, sculptTableTopRadius);
    pSurf sculptTopBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0,0, sculptTableHeight + 0.001));
    pSurf sculptTopTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0,0, sculptTableHeight + sculptTableTopThick));
    pSurf sculptTopSurf = sculptTopCyl & sculptTopBot & sculptTopTop;
    Surface::translate(sculptTopSurf, sculptTablePos);

    sculptTopSurf->setInnerMaterial(material::SILVER);

    pZone sculptZone(new StageZone("Sculpture Zone"));
    stageZone->addSubzone(sculptZone);
    sculptZone->setBounds(Box::boxPosDims(
        sculptTablePos + glm::dvec3(0, 0, (sculptTableHeight + sculptTableTopThick) /2.0 + 0.001),
        glm::dvec3(scultpTableRadius*3, scultpTableRadius*3, sculptTableHeight+sculptTableTopThick)));

    pProp sculptTableProp(new Prop("Sculpture Table"));
    sculptTableProp->addSurface(sculptTableSurf);
    sculptTableProp->addSurface(sculptTopSurf);
    sculptZone->addProp(sculptTableProp);


    ////////////////////
    // Light Fixtures //
    ////////////////////
    double lightFixtureHeight = 0.30;
    double lightFixtureOutRad = 0.10;
    double lightFixtureInRad = 0.08;
    pSurf lightFixtureOut = Quadric::cylinder(lightFixtureOutRad, lightFixtureOutRad);
    pSurf lightFixtureIn = Quadric::cylinder(lightFixtureInRad, lightFixtureInRad);
    pSurf lightFixtureMid = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf lightFixtureTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, lightFixtureHeight/2.0));
    pSurf lightFixtureBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0,-lightFixtureHeight/2.0));
    pSurf lightFixtureSurf = lightFixtureTop & lightFixtureBot &
                             lightFixtureOut & !(lightFixtureIn & lightFixtureMid);

    lightFixtureSurf->setInnerMaterial(material::TITANIUM);
    lightFixtureSurf->setCoating(lampCoat);

    glm::dvec3 fixturePositions[] = {
        glm::dvec3(boxMax.x*1/ 2.0, boxMax.y / 2.0, boxMax.z / 2.0 -  (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Hall
        glm::dvec3(boxMin.x*1/ 4.0, boxMax.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
        glm::dvec3(boxMin.x*3/ 4.0, boxMax.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
        glm::dvec3(boxMin.x*1/ 4.0, boxMin.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
        glm::dvec3(boxMin.x*3/ 4.0, boxMin.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
    };

    pMat cordMat = material::createInsulator(glm::dvec3(0.1), 1.44, 1.0, 1.0);
    for(const glm::dvec3& pos : fixturePositions)
    {
        pSurf surf = Surface::shell(lightFixtureSurf);
        Surface::translate(surf, pos);

        double cordTop;
        double cordSide = 0.004;
        if(pos.x < 0.0)
            cordTop = boxMax.z - wallThickness.z;
        else
            cordTop = boxMax.z/2.0 - wallThickness.z;
        pSurf cordSurf = Box::boxCorners(
            pos + glm::dvec3(-cordSide, -cordSide, lightFixtureHeight/2.0),
            pos + glm::dvec3( cordSide,  cordSide, cordTop - pos.z));
        cordSurf->setInnerMaterial(cordMat);

        pZone fixtureZone(new StageZone("Light Fixture Zone"));
        stageZone->addSubzone(fixtureZone);
        fixtureZone->setBounds(Box::boxCorners(
            pos + glm::dvec3(-lightFixtureOutRad, -lightFixtureOutRad, -lightFixtureHeight/2.0),
            pos + glm::dvec3( lightFixtureOutRad,  lightFixtureOutRad, cordTop - pos.z)));

        pProp lightFixturesProp(new Prop("Light Fixture"));
        lightFixturesProp->addSurface(surf);
        lightFixturesProp->addSurface(cordSurf);
        fixtureZone->addProp(lightFixturesProp);
    }
}


void CpuRaytracingCharacter::setupManufacturingStageSet()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 0);
    glm::dvec3 camPos = glm::dvec3(0, -2, 0);
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);


    auto env = play().propTeam3D()->stageSet()->environment();
    env->setBackdrop(std::shared_ptr<Backdrop>(new ProceduralSun(true)));

    // Setup
    pSurf floorSurf = Plane::plane(glm::dvec3(0.0, 0.0, 1.0), glm::dvec3());
    pProp floorProp(new Prop(""));
    floorSurf->setCoating(coating::CLEAR_ROUGH);
    floorProp->addSurface(floorSurf);

    glm::dvec3 color(0.97, 0.61, 0.51);
    double refractIndex = 1.38;
    double opacity = 0.7;

    // Grapefruit pulp
    pSurf pulpSurf = Sphere::sphere(glm::dvec3(-0.5, 0.0, 0.5), 0.5);
    pMat pulpMat = material::createInsulator(color, refractIndex, 0.9999, 0.2);
    pProp pulpProp(new Prop(""));
    pulpSurf->setCoating(coating::CLEAR_ROUGH);
    pulpSurf->setInnerMaterial(pulpMat);
    pulpProp->addSurface(pulpSurf);

    // Reference mat
    pSurf refSurf = Sphere::sphere(glm::dvec3( 0.5, 0.0, 0.5), 0.5);
    pMat refpMat = material::createInsulator(color, refractIndex, 1.0, 0.2);
    pProp refProp(new Prop(""));
    refSurf->setCoating(coating::CLEAR_ROUGH);
    refSurf->setInnerMaterial(refpMat);
    refProp->addSurface(refSurf);


    // Background prop
    pSurf backSurf = Sphere::sphere(glm::dvec3(0, 2.0, 0.3), 0.3);
    pProp backProp(new Prop(""));
    backProp->addSurface(backSurf);
}


std::shared_ptr<Surface> CpuRaytracingCharacter::createHoleStrippedWall(
        const glm::dvec3& size,
        double stripeWidth,
        double holeWidth,
        double border)
{

    double zmin = -size.x/2.0 + border;
    double zmax = size.z + size.x/2.0 - border;
    const glm::dvec3 normalUp = glm::normalize(glm::dvec3(-1, 0, 1));
    const glm::dvec3 normalDn = glm::normalize(glm::dvec3( 1, 0,-1));

    double holeOff = holeWidth / normalUp.z;
    double patternWidth = (stripeWidth + holeWidth) / normalUp.z;
    int patternCount = glm::round((zmax - zmin) / patternWidth);

    pSurf holes;
    double stripZ = size.z / 2.0 - ((patternCount-1)/2.0) * patternWidth;
    for(int i=0; i < patternCount; ++i)
    {
        pSurf holeUp = Plane::plane(normalUp, glm::dvec3(0, 0, stripZ + holeOff/2.0));
        pSurf holeDn = Plane::plane(normalDn, glm::dvec3(0, 0, stripZ - holeOff/2.0));

        if(holes.get() == nullptr)
            holes = holeUp & holeDn;
        else
            holes = holes | (holeUp & holeDn);

        stripZ += patternWidth;
    }

    pSurf wallBase = Box::boxCorners(
            glm::dvec3(-size.x/2, -size.y/2, 0),
            glm::dvec3(size.x/2, size.y/2, size.z));

    pSurf bordeSurf = Box::boxCorners(
        glm::dvec3(-size.x/2 + border, -size.y/2 - 0.001, border),
        glm::dvec3( size.x/2 - border,  size.y/2 + 0.001, size.z- border));

    pSurf wallSurf = wallBase & !(bordeSurf & holes);
    return wallSurf;
}
