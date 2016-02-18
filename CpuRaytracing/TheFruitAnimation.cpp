#include "TheFruitChoreographer.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Path/CompositePath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/LinearPath.h>

#include "Managers/PathManager.h"

using namespace cellar;


void TheFruitChoreographer::setupAnimation()
{
    // Camera Eye
    std::shared_ptr<CompositePath<glm::dvec3>> camEyePath(
        new CompositePath<glm::dvec3>());
    _cameraEyePath = camEyePath;

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
    _cameraToPath = camToPath;

    camToPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(9.0,
            glm::dvec3(4.0, 5.0, 3.60),
            glm::dvec3(-5.0, 5.0, 3.60)));
    camToPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(2.0,
            glm::dvec3(-5.0, 5.0, 3.60),
            glm::dvec3(-5.0, -7.0, 1.60)));


    // Camera Field of View
    std::shared_ptr<CompositePath<double>> cameraFoV(
        new CompositePath<double>());
    _cameraFoV = cameraFoV;

    cameraFoV->addPath(std::make_shared<LinearPath<double>>(9.25, 60.0, 60.0));
    cameraFoV->addPath(std::make_shared<LinearPath<double>>(0.75, 60.0, 35.0));
    cameraFoV->addPath(std::make_shared<LinearPath<double>>(4.00, 35.0, 60.0));


    // The Fruit Path
    std::shared_ptr<CompositePath<glm::dvec3>> theFruitPath(
        new CompositePath<glm::dvec3>());
    _theFruitPath = theFruitPath;

    theFruitPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(9.0,
            _theFruitPosition,
            _theFruitPosition));

    theFruitPath->addPath(std::make_shared<CubicSplinePath<glm::dvec3>>(10.0,
        std::vector<glm::dvec3>{
            _theFruitPosition,
            _theFruitPosition,
            glm::dvec3(7.0, -7.0, _theFruitPosition.z),
            glm::dvec3(7.0, 2.5, _theFruitPosition.z),
            glm::dvec3(-2.5, 2.5, _theFruitPosition.z)}));


    // Clouds
    std::shared_ptr<CompositePath<glm::dvec3>> cloudsPath(
        new CompositePath<glm::dvec3>());
    _cloudsPath = cloudsPath;

    cloudsPath->addPath(std::make_shared<LinearPath<glm::dvec3>>(45.0,
        _cloudsPosition, _cloudsPosition + glm::dvec3(0, 450.0, 0.0)));


    // Sun
    std::shared_ptr<CompositePath<double>> sunPath(
        new CompositePath<double>());
    _sunPath = sunPath;

    sunPath->addPath(std::make_shared<LinearPath<double>>(40.0, 10.0, 18.0));

    _animTotalTime = 21.0;
}

void TheFruitChoreographer::update(double dt)
{
    bool forcedUpdate = (dt == 0.0);

    if(forcedUpdate || (_animPlaying && (
       _animFastPlay || !_raytracerState->isRendering())))
    {
        double t = _animTime;

        const glm::dvec3 camUp(0, 0, 1);
        glm::dvec3 camTo = _cameraToPath->value(t);
        glm::dvec3 camEye = _cameraEyePath->value(t);
        _camera->updateView(glm::lookAt(camEye, camTo, camUp));


        float dofDist = glm::length(camTo - camEye);
        float dofAper = dofDist + _camAperture + 1.0;

        glm::mat4 projection =
            glm::perspectiveFov(
                glm::radians((float)_cameraFoV->value(t))/2,
                (float) _camera->viewport().x,
                (float) _camera->viewport().y,
                dofDist, dofAper);
        _camera->updateProjection(projection);

        glm::dvec3 theFruitNewPos = _theFruitPath->value(t);
        glm::dvec3 theFruitDisplacement = theFruitNewPos - _theFruitPosition;
        _theFruitProp->translate(theFruitDisplacement);
        _theFruitPosition = theFruitNewPos;


        glm::dvec3 cloudsNewPos = _cloudsPath->value(t);
        glm::dvec3 cloudsDisplacement = cloudsNewPos - _cloudsPosition;
        _cloudsZone->translate(cloudsDisplacement);
        _cloudsPosition = cloudsNewPos;


        if(!forcedUpdate)
        {
            if(_animFastPlay)
            {
                _animTime += dt;
                _animFrame = _animTime * _animFps;
            }
            else
            {
                ++_animFrame;
                _animTime = _animFrame / double(_animFps);
            }

            emit animFrameChanged(_animFrame);
            if(_animTime >= _animTotalTime)
            {
                _animPlaying = false;
                playStateChanged(_animPlaying);
            }
        }
    }
}

int TheFruitChoreographer::animFrameCount()
{
    return glm::ceil(_animTotalTime * _animFps);
}

void TheFruitChoreographer::setAnimFps(int fps)
{
    _animFps = fps;
}

void TheFruitChoreographer::setAnimFrame(int frame)
{
    if(_animFrame != frame)
    {
        _animFrame = frame;
        _animTime = frame / double(_animFps);

        emit animFrameChanged(_animFrame);

        update(0.0);
    }
}

void TheFruitChoreographer::resetAnimation()
{
    setAnimFrame(0);
}

void TheFruitChoreographer::playAnimation()
{
    _animPlaying = true;
}

void TheFruitChoreographer::pauseAnimation()
{
    _animPlaying = false;
}

void TheFruitChoreographer::setFastPlay(bool playFast)
{
    _animFastPlay = playFast;

    if(_animPlaying)
        update(0.0);
}

void TheFruitChoreographer::displayPaths(PathManager& pathManager)
{
    pathManager.clearPaths();

    pathManager.appendPath(_cameraToPath,   "Camera To");
    pathManager.appendPath(_cameraEyePath,  "Camera Eye");
    pathManager.appendPath(_cameraFoV,      "Camera FoV");

    pathManager.appendPath(_theFruitPath,   "The Fruit Position");

    pathManager.appendPath(_cloudsPath,     "Clouds Position");

    pathManager.appendPath(_sunPath,        "Day Time");
}
