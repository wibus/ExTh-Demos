#include "Physics2DCharacter.h"

#include <iostream>
using namespace std;

#include <CellarWorkbench/Misc/CellarUtils.h>
using namespace cellar;

#include <PropRoom2D/Shape/Circle.h>
#include <PropRoom2D/Shape/Polygon.h>
#include <PropRoom2D/Hud/TextHud.h>
#include <PropRoom2D/Hud/ImageHud.h>
#include <PropRoom2D/PropTeam/AbstractPropTeam.h>
#include <PropRoom2D/Costume/CircleCostume.h>
#include <PropRoom2D/Costume/PolygonCostume.h>
#include <PropRoom2D/Hardware/Hardware.h>
using namespace prop2;

#include <Scaena/Stage/AbstractStage.h>
#include <Scaena/Stage/Event/StageTime.h>
using namespace scaena;


Physics2DCharacter::Physics2DCharacter(AbstractStage &stage) :
    AbstractCharacter(stage, "Character")
{
}

void Physics2DCharacter::enterStage()
{
    stage().propTeam2D().setGravity(glm::dvec2(0.0f, -98.0f));

    if(!_background)
    {
        const vector<glm::dvec2> backgroundVertices = {
            glm::dvec2(0.0,              0.0),
            glm::dvec2(stage().width(),  0.0),
            glm::dvec2(stage().width(),  stage().height()),
            glm::dvec2(0.0,              stage().height())
        };
        _background = stage().propTeam2D().createPolygon(4);
        _background->setBodyType(EBodyType::GRAPHIC);
        _background->setVertices(backgroundVertices);
        _background->costume()->setTextureName(":/Physics2D/textures/campagne.png");
        _background->costume()->setDepth(-1.0);
    }

    if(!_groundFloor)
    {
        const vector<glm::dvec2> groundFloorTexCoords = {
            glm::dvec2(0.0, 0.0),
            glm::dvec2(30.0, 0.0),
            glm::dvec2(30.0, 1.0),
            glm::dvec2(0.0, 1.0)
        };
        std::shared_ptr<PolygonCostume> groundFloorCostume(
            new PolygonCostume(groundFloorTexCoords.size()));
        groundFloorCostume->setVerticesTexCoords(groundFloorTexCoords);
        groundFloorCostume->setTextureName(":/Physics2D/textures/ground.bmp");

        const vector<glm::dvec2> floorVertices = {
            glm::dvec2(0.0,   0.0),
            glm::dvec2(800.0, 0.0),
            glm::dvec2(800.0, 20.0),
            glm::dvec2(0.0,   20.0)
        };
        _groundFloor = stage().propTeam2D().createPolygon(4);
        _groundFloor->setVertices(floorVertices);
        _groundFloor->setBodyType(EBodyType::KINEMATIC);
        _groundFloor->setCostume(groundFloorCostume);
    }

    if(!_groundLeft && !_groundRight)
    {
        const vector<glm::dvec2> groundSidesTexCoords = {
            glm::dvec2(0.0, 0.0),
            glm::dvec2(1.0, 0.0),
            glm::dvec2(1.0, 30.0),
            glm::dvec2(0.0, 30.0)
        };
        std::shared_ptr<PolygonCostume> groundSidesCostume(
            new PolygonCostume(groundSidesTexCoords.size()));
        groundSidesCostume->setVerticesTexCoords(groundSidesTexCoords);
        groundSidesCostume->setTextureName(":/Physics2D/textures/ground.bmp");

        const vector<glm::dvec2> groundSides = {
            glm::dvec2(0.0,  0.0),
            glm::dvec2(20.0, 0.0),
            glm::dvec2(20.0, 580.0),
            glm::dvec2(0.0,  580.0)
        };
        _groundLeft = stage().propTeam2D().createPolygon(groundSides.size());
        _groundLeft->setVertices(groundSides);
        _groundLeft->moveVertexAt(0, _groundFloor->outline()[3].begin());
        _groundLeft->setBodyType(EBodyType::KINEMATIC);
        _groundLeft->setCostume(groundSidesCostume);

        _groundRight = stage().propTeam2D().createPolygon(groundSides.size());
        _groundRight->setVertices(groundSides);
        _groundRight->moveVertexAt(1, _groundFloor->outline()[2].begin());
        _groundRight->setBodyType(EBodyType::KINEMATIC);
        _groundRight->setCostume(groundSidesCostume);
    }


    if(!_ramp)
    {
        std::vector<glm::dvec2> rampVertices;
        rampVertices.push_back(glm::dvec2(0, 0));

        float x, y;
        for(int i=0; i < 200; ++i)
        {
            x = -i;
            y = (200.0f / (i+1)) + (i-120)*(i-120)/200.0f;
            rampVertices.push_back(glm::dvec2(x, y));
        }
        rampVertices.push_back(glm::dvec2(x, 0));
        _ramp = stage().propTeam2D().createPolygon(rampVertices.size());
        _ramp->setVertices(rampVertices);
        _ramp->moveVertexAt(0, _groundRight->outline()[0].begin());
        _ramp->setBodyType(EBodyType::KINEMATIC);
    }

    std::shared_ptr<Hardware> houseMaterial(new Hardware());
    houseMaterial->setBounciness(0.4);
    houseMaterial->setStaticFrictionCoefficient(0.4);
    houseMaterial->setDynamicFrictionCoefficient(0.3);

    if(!_housePillarLeft && !_housePillarRight)
    {
        std::vector<glm::dvec2> pillarTexCoords = {
            glm::dvec2(0.0,  0.0),
            glm::dvec2(0.25, 0.0),
            glm::dvec2(0.25, 1.0),
            glm::dvec2(0.0,  1.0)
        };

        std::shared_ptr<PolygonCostume> pillarCostume(
            new PolygonCostume(pillarTexCoords.size()));
        pillarCostume->setTextureName(":/Physics2D/textures/ball1.bmp");
        pillarCostume->setVerticesTexCoords(pillarTexCoords);

        std::vector<glm::dvec2> pillarVertices = {
            glm::dvec2(0.0,  0.0),
            glm::dvec2(30.0, 0.0),
            glm::dvec2(30.0, 120.0),
            glm::dvec2(0.0,  120.0)
        };

        _housePillarLeft = stage().propTeam2D().createPolygon(pillarVertices.size());
        _housePillarLeft->setVertices(pillarVertices);
        _housePillarLeft->moveVertexAt(0, _groundFloor->outline()[3].begin() + glm::dvec2(160.0, 0.0));
        _housePillarLeft->setBodyType(EBodyType::DYNAMIC);
        _housePillarLeft->setCostume(pillarCostume);
        _housePillarLeft->setHardware(houseMaterial);

        _housePillarRight = stage().propTeam2D().createPolygon(pillarVertices.size());
        _housePillarRight->setVertices(pillarVertices);
        _housePillarRight->moveVertexAt(0, _groundFloor->outline()[3].begin() + glm::dvec2(260.0, 0.0));
        _housePillarRight->setBodyType(EBodyType::DYNAMIC);
        _housePillarRight->setCostume(pillarCostume);
        _housePillarRight->setHardware(houseMaterial);
    }

    if(!_houseRoof)
    {
        std::vector<glm::dvec2> houseRoofVertices = {
            glm::dvec2(-100.0, 0.0),
            glm::dvec2( 100.0, 0.0),
            glm::dvec2( 100.0, 20.0),
            glm::dvec2( 0.0,   60.0),
            glm::dvec2(-100,   20.0),
        };
        std::vector<glm::dvec2> houseRoofTexCoords = {
            glm::dvec2(0.0, 0.0),
            glm::dvec2(1.0, 0.0),
            glm::dvec2(1.0, 0.1),
            glm::dvec2(0.5, 0.3),
            glm::dvec2(0.0, 0.1),
        };

        _houseRoof = stage().propTeam2D().createPolygon(houseRoofVertices.size());
        _houseRoof->setVertices(houseRoofVertices);
        _houseRoof->moveBy(glm::dvec2(225, 140));
        _houseRoof->setBodyType(EBodyType::DYNAMIC);
        _houseRoof->costume()->setTextureName(":/Physics2D/textures/ball1.bmp");
        _houseRoof->costume()->setVerticesTexCoords(houseRoofTexCoords);
        _houseRoof->setHardware(houseMaterial);
    }

    if(!_gear)
    {
        int nbTeeth = 10;
        double innerRadius = 20;
        double outerRadius = 25;

        std::vector<glm::dvec2> gearVertices;
        std::vector<glm::dvec4> gearColors;
        for(int i=0; i < nbTeeth; ++i)
        {
            glm::dvec2 vec1(cos(2.0*PI*double(i)/nbTeeth),
                       sin(2.0*PI*double(i)/nbTeeth));
            glm::dvec2 vec2(cos(2.0*PI*double(i+0.5)/nbTeeth),
                       sin(2.0*PI*double(i+0.5)/nbTeeth));
            gearVertices.push_back(vec1 * outerRadius);
            gearVertices.push_back(vec1 * innerRadius);
            gearVertices.push_back(vec2 * innerRadius);
            gearVertices.push_back(vec2 * outerRadius);

            double perc = double(i)/nbTeeth;
            gearColors.push_back(glm::dvec4(0.3, 0.3, 0.3+0.7*(perc-0.5)*(perc-0.5), 1.0));
            gearColors.push_back(glm::dvec4(0.8, 0.2, 0.0, 1.0));
            gearColors.push_back(glm::dvec4(0.8, 0.2, 0.0, 1.0));
            gearColors.push_back(glm::dvec4(0.3, 0.3, 0.3+0.7*(perc-0.5)*(perc-0.5), 1.0));
        }

        _gear = stage().propTeam2D().createPolygon(gearVertices.size());
        _gear->setVertices(gearVertices);
        _gear->moveBy(_ramp->outline()[0].begin() + glm::dvec2(-outerRadius, 600.0));
        _gear->setBodyType(EBodyType::DYNAMIC);
        _gear->hardware()->setBounciness(0.6);
        _gear->costume()->setVerticesColors(gearColors);
    }

    if(!_ball)
    {
        _ball = stage().propTeam2D().createCircle();
        _ball->setRadius(30);
        _ball->setCenter(_ramp->outline()[0].begin() + glm::dvec2(-_ball->radius()-600, 400.0));
        _ball->setBodyType(EBodyType::DYNAMIC);
        _ball->setAngularVelocity(2.0);
        _ball->setLinearFrictionCoefficients(glm::dvec3(0.0, 0.5, 0.0));
        _ball->setAngularFrictionCoefficients(glm::dvec3(0.05, 0.0, 0.0));
        _ball->costume()->setTextureName(":/Physics2D/textures/face.bmp");
    }



    _statsPanel = stage().propTeam2D().createImageHud();
    _statsPanel->setSize(glm::dvec2(110, 48));
    _statsPanel->setHandlePosition(glm::dvec2(10, -_statsPanel->height() - 10));
    _statsPanel->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _statsPanel->setVerticalAnchor(EVerticalAnchor::TOP);
    _statsPanel->setImageName(":/Physics2D/textures/statsPanel.bmp");
    _statsPanel->setTexOrigin(glm::dvec2(0.0, 0.0));
    _statsPanel->setTexExtents(glm::dvec2(1.0, 1.0));

    _fps = stage().propTeam2D().createTextHud();
    _fps->setText("FPS: ");
    _fps->setHandlePosition(glm::dvec2(18.0, -30.0));
    _fps->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _fps->setVerticalAnchor(EVerticalAnchor::TOP);
    _fps->setHeight(15);

    _ups = stage().propTeam2D().createTextHud();
    _ups->setText("UPS: ");
    _ups->setHandlePosition(glm::dvec2(18.0, -50.0));
    _ups->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _ups->setVerticalAnchor(EVerticalAnchor::TOP);
    _ups->setColor(glm::dvec4(0.3, 0.4, 1.0, 1.0));
    _ups->setHeight(15);
}

void Physics2DCharacter::beginStep(const StageTime &time)
{
    _ups->setText("UPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void Physics2DCharacter::draw(const StageTime &time)
{
    _fps->setText("FPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void Physics2DCharacter::exitStage()
{
    stage().propTeam2D().deletePolygon(_background);
    stage().propTeam2D().deletePolygon(_groundFloor);
    stage().propTeam2D().deletePolygon(_groundLeft);
    stage().propTeam2D().deletePolygon(_groundRight);
    stage().propTeam2D().deletePolygon(_ramp);
    stage().propTeam2D().deletePolygon(_housePillarLeft);
    stage().propTeam2D().deletePolygon(_housePillarRight);
    stage().propTeam2D().deletePolygon(_houseRoof);
    stage().propTeam2D().deletePolygon(_gear);
    stage().propTeam2D().deleteCircle(_ball);
    stage().propTeam2D().deleteImageHud(_statsPanel);
    stage().propTeam2D().deleteTextHud(_fps);
    stage().propTeam2D().deleteTextHud(_ups);
}
