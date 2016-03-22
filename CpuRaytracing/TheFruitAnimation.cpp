#include "TheFruitChoreographer.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <QDir>

#include <CellarWorkbench/Misc/Log.h>
#include <CellarWorkbench/Image/Image.h>
#include <CellarWorkbench/GL/GlToolkit.h>

#include <PropRoom3D/Node/StageZone.h>
#include <PropRoom3D/Node/Prop/Prop.h>
#include <PropRoom3D/Node/Prop/Surface/Surface.h>
#include <PropRoom3D/Node/Debug/DebugLineStrip.h>
#include <PropRoom3D/Node/Light/Backdrop/ProceduralSun.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

#include "Model/PathModel.h"
#include "Model/SceneDocument.h"
#include "Managers/PathManager.h"

using namespace cellar;
using namespace prop3;

const std::string RECORD_OUPUT_PREFIX = "CpuRaytracing/Animations/";

void TheFruitChoreographer::update(double dt)
{
    bool forcedUpdate = (dt == 0.0);

    if(forcedUpdate || (_animPlaying && (
       _animFastPlay || !_raytracerState->isRendering())))
    {
        if(_isRecording && !forcedUpdate && !_animFastPlay)
            saveCurrentFrame();

        double t = _animTime;

        if(!_cameraIsFree)
        {
            // Camera position
            const glm::dvec3 camUp(0, 0, 1);
            glm::dvec3 camTo = _pathModel->cameraTo->value(t);
            glm::dvec3 camEye = _pathModel->cameraEye->value(t);
            _camera->updateView(glm::lookAt(camEye, camTo, camUp));

            // Camera focus
            float dofDist = glm::length(camTo - camEye);
            float dofAper = dofDist + _camAperture + ArtDirectorServer::IMAGE_DEPTH;
            glm::mat4 projection =
                glm::perspectiveFov(
                    glm::radians((float)_pathModel->cameraFoV->value(t))/2,
                    (float) _camera->viewport().x,
                    (float) _camera->viewport().y,
                    dofDist, dofAper);
            _camera->updateProjection(projection);
        }

        // The Fruit's position
        glm::dvec3 theFruitNewPos = _pathModel->theFruitPos->value(t);
        glm::dvec3 theFruitDisplacement = theFruitNewPos - _theFruitPosition;
        _theFruitZone->translate(theFruitDisplacement);
        _theFruitPosition = theFruitNewPos;

        // The Fruit's size
        double theFruitNewHeight = _pathModel->theFruitHeight->value(t);
        double theFruitDHeight = theFruitNewHeight / _theFruitHeight;
        double theFruitDWidth = 1.0 / glm::sqrt(theFruitDHeight);
        Surface::transform(_theFruitSurf, glm::scale(glm::dmat4(), glm::dvec3(
            theFruitDWidth, theFruitDWidth, theFruitDHeight)));
        _theFruitHeight = theFruitNewHeight;


        // Clouds position
        glm::dvec3 cloudsNewPos = _pathModel->clouds->value(t);
        glm::dvec3 cloudsDisplacement = cloudsNewPos - _cloudsPosition;
        _cloudsZone->translate(cloudsDisplacement);
        _cloudsPosition = cloudsNewPos;


        // Sin direction
        glm::dvec3 lastPos(-2.4, -1.4, 0.8);
        glm::dvec3 lastHole(5.0, -4.0, 2.5);
        glm::dvec3 sunAxis = glm::normalize(glm::dvec3(1, 3, 0.2));
        glm::dvec4 lastDir(glm::normalize(lastPos - lastHole), 0.0);
        double lastHour = _pathModel->dayTime->value(_pathModel->dayTime->duration());
        double sunRot = (_pathModel->dayTime->value(t) - lastHour) * glm::pi<double>() / 12.0;
        glm::dvec3 sunDir = glm::dvec3(glm::rotate(glm::dmat4(), sunRot, sunAxis) * lastDir);
        _backdrop->setSunDirection( -sunDir );


        if(!forcedUpdate)
        {
            if(!_animFastPlay)
            {
                ++_animFrame;
                _animTime = _animFrame / double(_animFps);
                forceUpdate();
            }
            else
            {
                _animTime += dt;
                _animFrame = _animTime * _animFps;
            }

            emit animFrameChanged(_animFrame);
            if(_animTime >= _pathModel->animationLength())
            {
                _animPlaying = false;
                playStateChanged(_animPlaying);
            }
        }
    }
}

void TheFruitChoreographer::forceUpdate()
{
    update(0.0);
}

int TheFruitChoreographer::animFrameCount()
{
    return glm::ceil(_pathModel->animationLength() * _animFps);
}

void TheFruitChoreographer::setAnimTimeOffset(double offset)
{
    // Anim time offset doesn't affect frame timings
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

        forceUpdate();
    }
}

void TheFruitChoreographer::resetAnimation()
{
    setAnimFrame(0);
}

void TheFruitChoreographer::startRecording()
{
    _isRecording = true;

    QString outputDir((RECORD_OUPUT_PREFIX +
       getSceneDocument().sceneName() + "/" +
       getSceneDocument().outputFrameDirectory()).c_str());

    QDir dir = QDir::current();
    dir.mkpath(outputDir);
}

void TheFruitChoreographer::stopRecording()
{
    _isRecording = false;
}

void TheFruitChoreographer::playAnimation()
{
    _animPlaying = true;
    forceUpdate();
}

void TheFruitChoreographer::pauseAnimation()
{
    _animPlaying = false;
}

void TheFruitChoreographer::setFastPlay(bool playFast)
{
    _animFastPlay = playFast;

    if(_animPlaying)
        forceUpdate();
}

void TheFruitChoreographer::bindCameraToPath()
{
    _cameraIsFree = false;
    forceUpdate();
}

void TheFruitChoreographer::freeCameraFromPath()
{
    _cameraIsFree = true;
}

std::shared_ptr<PathModel> TheFruitChoreographer::pathModel() const
{
    return _pathModel;
}

void TheFruitChoreographer::saveCurrentFrame()
{
    std::string animDir =
            getSceneDocument().sceneName() + "/" +
            getSceneDocument().outputFrameDirectory() + "/";
    QString fileName = (RECORD_OUPUT_PREFIX + animDir).c_str();

    fileName += QString("%1").arg(_animFrame, 4, 10, QChar('0'));
    if(getSceneDocument().includeSampleCountInFrame())
        fileName += QString("_%1f").arg(_raytracerState->sampleCount());
    if(getSceneDocument().includeRenderTimeInFrame())
        fileName += QString("_%1").arg(SceneDocument::timeToString(_raytracerState->renderTime()).c_str());
    if(getSceneDocument().includeDivergenceInFrame())
        fileName += "_"+QString::number(_raytracerState->divergence(), 'f', 4)+"div";
    fileName += getSceneDocument().outputFrameFormat().c_str();
    std::string stdFileName = fileName.toStdString();

    cellar::Image screenshot;
    cellar::GlToolkit::takeFramebufferShot(screenshot);
    screenshot.save(stdFileName);

    getLog().postMessage(new Message('I', false,
        stdFileName + " successfully recorded", "TheFruitChoreographer"));
}
