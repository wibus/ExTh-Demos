#include "TheFruitChoreographer.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Path/CompositePath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/LinearPath.h>

using namespace cellar;


void TheFruitChoreographer::setupAnimation()
{
    // Camera Eye
    std::shared_ptr<CompositePath<glm::dvec3>> camEyePath(
        new CompositePath<glm::dvec3>());
    _cameraEyePath = camEyePath;

    camEyePath->addPath(11.0, std::make_shared<LinearPath<glm::dvec3>>(
            glm::dvec3(4.0, 10.0, 3.60),
            glm::dvec3(-7.0, 10.0, 3.60)));

    camEyePath->addPath(10.0, std::make_shared<CubicSplinePath<glm::dvec3>>(
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

    camToPath->addPath(9.0, std::make_shared<LinearPath<glm::dvec3>>(
            glm::dvec3(4.0, 5.0, 3.60),
            glm::dvec3(-5.0, 5.0, 3.60)));
    camToPath->addPath(2.0, std::make_shared<LinearPath<glm::dvec3>>(
            glm::dvec3(-5.0, 5.0, 3.60),
            glm::dvec3(-5.0, -7.0, 1.60)));


    // Camera Field of View
    std::shared_ptr<CompositePath<double>> cameraFoV(
        new CompositePath<double>());
    _cameraFoV = cameraFoV;

    cameraFoV->addPath(9.25, std::make_shared<LinearPath<double>>(60.0, 60.0));
    cameraFoV->addPath(0.75, std::make_shared<LinearPath<double>>(60.0, 35.0));
    cameraFoV->addPath(4.00, std::make_shared<LinearPath<double>>(35.0, 60.0));


    // The Fruit Path
    std::shared_ptr<CompositePath<glm::dvec3>> theFruitPath(
        new CompositePath<glm::dvec3>());
    _theFruitPath = theFruitPath;

    theFruitPath->addPath(9.0, std::make_shared<LinearPath<glm::dvec3>>(
            _theFruitPosition,
            _theFruitPosition));

    theFruitPath->addPath(10.0, std::make_shared<CubicSplinePath<glm::dvec3>>(
        std::vector<glm::dvec3>{
            _theFruitPosition,
            _theFruitPosition,
            glm::dvec3(7.0, -7.0, _theFruitPosition.z),
            glm::dvec3(7.0, 2.5, _theFruitPosition.z),
            glm::dvec3(-2.5, 2.5, _theFruitPosition.z)}));

    _cloudsVelocity = glm::dvec3(0.0, 10.0, 0.0);

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


        _cloudsZone->translate(_cloudsVelocity * dt);


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
