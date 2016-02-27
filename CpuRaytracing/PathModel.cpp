#include "PathModel.h"

#include <CellarWorkbench/Path/CompositePath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/LinearPath.h>

#include <PropRoom3D/Node/StageSet.h>
#include <PropRoom3D/Node/Debug/DebugLine.h>

using namespace cellar;


const std::string PathModel::CAMERA_EYE_PATH_NAME   = "Camera Eye";
const std::string PathModel::CAMERA_TO_PATH_NAME    = "Camera To";
const std::string PathModel::CAMERA_FOV_PATH_NAME   = "Camera Fov";
const std::string PathModel::THE_FRUIT_PATH_NAME    = "The Fruit";
const std::string PathModel::DAY_TIME_PATH_NAME     = "Day Time";
const std::string PathModel::CLOUDS_PATH_NAME       = "Clouds";

PathModel::PathModel()
{

}

PathModel::~PathModel()
{

}

void PathModel::init(const std::shared_ptr<prop3::StageSet>& stageSet)
{
    _stageSet = stageSet;

    // Camera Eye
    std::shared_ptr<CompositePath<glm::dvec3>> camEyePath(
        new CompositePath<glm::dvec3>());
    cameraEye = camEyePath;

    camEyePath->addPath(std::make_shared<LinearPath<glm::dvec3>>(11.0,
            glm::dvec3(4.0, 10.0, 3.60),
            glm::dvec3(-7.0, 10.0, 3.60)));

    camEyePath->addPath(std::make_shared<CubicSplinePath<glm::dvec3>>(10.0,
        std::vector<glm::dvec3>{
            glm::dvec3(-7.0, 11.0,  3.60),
            glm::dvec3(-7.0, 10.0,  3.60),
            glm::dvec3(-7.0, 0.0,   1.70),
            glm::dvec3(-10.0, -7.0,  2.60),
            glm::dvec3(-15.0, -12.0, 5.0),
            glm::dvec3(-15.0, -18.0, 8.0),
            glm::dvec3(-10.0, -12.0, 5.0),
            glm::dvec3(-6.0, -8.0,   2.0),
            glm::dvec3(-4.0, -6.5,   1.70),
            glm::dvec3(1.0, -6.0,   1.70),
            glm::dvec3(5.0, -6.0,   1.70)}));


    // Camera To
    std::shared_ptr<CompositePath<glm::dvec3>> camToPath(
        new CompositePath<glm::dvec3>());
    cameraTo = camToPath;

    camToPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(9.0,
            glm::dvec3(4.0, 5.0, 3.60),
            glm::dvec3(-5.0, 5.0, 3.60)));
    camToPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(2.0,
            glm::dvec3(-5.0, 5.0, 3.60),
            glm::dvec3(-5.0, -7.0, 1.60)));


    // Camera Field of View
    std::shared_ptr<CompositePath<double>> camFoVPath(
        new CompositePath<double>());
    cameraFoV = camFoVPath;

    camFoVPath->addPath(std::make_shared<LinearPath<double>>(9.25, 60.0, 60.0));
    camFoVPath->addPath(std::make_shared<LinearPath<double>>(0.75, 60.0, 35.0));
    camFoVPath->addPath(std::make_shared<LinearPath<double>>(4.00, 35.0, 60.0));


    // The Fruit Path
    std::shared_ptr<CompositePath<glm::dvec3>> theFruitPath(
        new CompositePath<glm::dvec3>());
    theFruit = theFruitPath;

    theFruitPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(9.0,
            glm::dvec3(-6.0, -7.0, 0.0),
            glm::dvec3(-6.0, -7.0, 0.0)));

    theFruitPath->addPath(std::make_shared<CubicSplinePath<glm::dvec3>>(10.0,
        std::vector<glm::dvec3>{
            glm::dvec3(-6.0, -7.0, 0.0),
            glm::dvec3(-6.0, -7.0, 0.0),
            glm::dvec3(7.0, -7.0, 0.0),
            glm::dvec3(7.0, 2.5, 0.0),
            glm::dvec3(-2.5, 2.5, 0.0)}));


    // Clouds
    std::shared_ptr<CompositePath<glm::dvec3>> cloudsPath(
        new CompositePath<glm::dvec3>());
    clouds = cloudsPath;

    cloudsPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(45.0,
        glm::dvec3(-200.0, -150.0, 50.0),
        glm::dvec3(-200.0, -150.0, 50.0) +
            glm::dvec3(0, 450.0, 0.0)));


    // Sun
    std::shared_ptr<CompositePath<double>> sunPath(
        new CompositePath<double>());
    dayTime = sunPath;

    sunPath->addPath(std::make_shared<LinearPath<double>>(40.0, 10.0, 18.0));

    refreshDebugLines();
}

double PathModel::animationLength() const
{
    double maxLength = 0.0;
    maxLength = glm::max(maxLength, cameraEye->duration());
    maxLength = glm::max(maxLength, cameraTo->duration());
    maxLength = glm::max(maxLength, cameraFoV->duration());
    maxLength = glm::max(maxLength, theFruit->duration());
    maxLength = glm::max(maxLength, clouds->duration());
    maxLength = glm::max(maxLength, dayTime->duration());

    return maxLength;
}

void PathModel::refreshDebugLines()
{
    const double DEBUG_FPS = 24.0;

    _stageSet->clearDebugLines();

    if(_lineVisibility[CAMERA_EYE_PATH_NAME])
    {
        double duration = cameraEye->duration();
        prop3::DebugLine camEyeLine(glm::dvec3(0, 1, 0));
        for(double t=0.0; t < duration; t+=1.0/DEBUG_FPS)
            camEyeLine.addVertex(cameraEye->value(t));
        _stageSet->addDebugLine(camEyeLine);
    }

    if(_lineVisibility[CAMERA_TO_PATH_NAME])
    {
        double duration = cameraTo->duration();
        prop3::DebugLine camToLine(glm::dvec3(0, 0, 1));
        for(double t=0.0; t < duration; t+=1.0/DEBUG_FPS)
            camToLine.addVertex(cameraTo->value(t));
        _stageSet->addDebugLine(camToLine);
    }
}

bool PathModel::isDebugLineVisible(const std::string& name)
{
    return _lineVisibility[name];
}

void PathModel::setDebugLineVisibility(const std::string& name, bool isVisible)
{
    _lineVisibility[name] = isVisible;

    refreshDebugLines();
}
