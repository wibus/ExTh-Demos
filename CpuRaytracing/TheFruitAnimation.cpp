#include "TheFruitChoreographer.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <QDir>

#include <CellarWorkbench/Misc/Log.h>
#include <CellarWorkbench/Image/Image.h>
#include <CellarWorkbench/GL/GlToolkit.h>

#include <PropRoom3D/Node/StageZone.h>
#include <PropRoom3D/Node/Prop/Prop.h>
#include <PropRoom3D/Node/Debug/DebugLine.h>

#include "Paths/PathModel.h"
#include "Managers/PathManager.h"

using namespace cellar;

const std::string RECORD_OUPUT_PREFIX = "CpuRaytracing/Animations/";

void TheFruitChoreographer::update(double dt)
{
    bool forcedUpdate = (dt == 0.0);

    if(forcedUpdate || (_animPlaying && (
       _animFastPlay || !_raytracerState->isRendering())))
    {
        double t = _animTime;

        const glm::dvec3 camUp(0, 0, 1);
        glm::dvec3 camTo = _pathModel->cameraTo->value(t);
        glm::dvec3 camEye = _pathModel->cameraEye->value(t);
        _camera->updateView(glm::lookAt(camEye, camTo, camUp));


        float dofDist = glm::length(camTo - camEye);
        float dofAper = dofDist + _camAperture + 1.0;

        glm::mat4 projection =
            glm::perspectiveFov(
                glm::radians((float)_pathModel->cameraFoV->value(t))/2,
                (float) _camera->viewport().x,
                (float) _camera->viewport().y,
                dofDist, dofAper);
        _camera->updateProjection(projection);

        glm::dvec3 theFruitNewPos = _pathModel->theFruit->value(t);
        glm::dvec3 theFruitDisplacement = theFruitNewPos - _theFruitPosition;
        _theFruitProp->translate(theFruitDisplacement);
        _theFruitPosition = theFruitNewPos;


        glm::dvec3 cloudsNewPos = _pathModel->clouds->value(t);
        glm::dvec3 cloudsDisplacement = cloudsNewPos - _cloudsPosition;
        _cloudsZone->translate(cloudsDisplacement);
        _cloudsPosition = cloudsNewPos;


        if(!forcedUpdate)
        {
            if(!_animFastPlay)
            {
                if(_isRecording)
                    saveCurrentFrame();

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

    QString outputDir((RECORD_OUPUT_PREFIX + _recordOutput.name).c_str());

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

std::shared_ptr<PathModel> TheFruitChoreographer::pathModel() const
{
    return _pathModel;
}

RecordOutput& TheFruitChoreographer::recordOutput()
{
    return _recordOutput;
}

void TheFruitChoreographer::saveCurrentFrame()
{
    QString fileName = (RECORD_OUPUT_PREFIX + _recordOutput.name + "/").c_str();
    fileName += QString("%1").arg(_animFrame, 4, 10, QChar('0'));
    if(_recordOutput.includeSampleCount)
        fileName += QString("_%1f").arg(_raytracerState->sampleCount());
    if(_recordOutput.includeRenderTime)
        fileName += QString("_%1s").arg((int)_raytracerState->renderTime());
    if(_recordOutput.includeDivergence)
        fileName += "_"+QString::number(_raytracerState->divergence(), 'f', 4)+"div";
    fileName += _recordOutput.format.c_str();
    std::string stdFileName = fileName.toStdString();

    cellar::Image screenshot;
    cellar::GlToolkit::takeFramebufferShot(screenshot);
    screenshot.save(stdFileName);

    getLog().postMessage(new Message('I', false,
        stdFileName + " successfully recorded", "TheFruitChoreographer"));
}
