#include "SceneDocument.h"

#include <QString>


SceneDocument& getSceneDocument()
{
    return *SceneDocument::getInstance();
}

SceneDocument::SceneDocument() :
    _rootDir("CpuRaytracing/Animations/"),
    _sceneName("Scene"),
    _stageSetName("StageSet"),
    _outputFrameDirectory("frames"),
    _outputFrameFormat(".png"),
    _includeSampleCountInFrame(true),
    _includeRenderTimeInFrame(true),
    _includeDivergenceInFrame(true)
{

}

SceneDocument::~SceneDocument()
{

}

void SceneDocument::setDocumentRootDirectory(const std::string& root)
{
    _rootDir = root;
}

void SceneDocument::setSceneName(const std::string& name)
{
    _sceneName = name;
}

void SceneDocument::setStageSetName(const std::string& name)
{
    _stageSetName = name;
}

void SceneDocument::setOutputFrameDirectory(const std::string& directory)
{
    _outputFrameDirectory = directory;
}

void SceneDocument::setOutputFrameFormat(const std::string& format)
{
    _outputFrameFormat = format;
}

void SceneDocument::setIncludeSampleCountInFrame(bool include)
{
    _includeSampleCountInFrame = include;
}

void SceneDocument::setIncludeRenderTimeInFrame(bool include)
{
    _includeRenderTimeInFrame = include;
}

void SceneDocument::setIncludeDivergenceInFrame(bool include)
{
    _includeDivergenceInFrame = include;
}

std::string SceneDocument::getStageSetFilePath() const
{
    return _rootDir + _sceneName + "/" + _stageSetName + ".prop3";
}

std::string SceneDocument::getAnimationPathsFilePath() const
{
    return _rootDir + _sceneName + "/Paths.pth";
}

std::string SceneDocument::getTextureRootDirectory() const
{
    return _rootDir + _sceneName + "/textures/";
}

std::string SceneDocument::getAnimationFramesDirectory() const
{
    return _rootDir + _sceneName + "/" + _outputFrameDirectory + "/";
}

const double SEC_IN_MIN = 60.0;
const double MIL_IN_SEC = 1000.0;
std::string SceneDocument::timeToString(double time)
{
    int minutes = int(time / SEC_IN_MIN);
    double minToSec = minutes * SEC_IN_MIN;
    int seconds = int(time - minToSec);
    int millisec = int((time - minToSec - seconds)*MIL_IN_SEC);
    QString str = QString("%1m%2s%3l")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(millisec, 3, 10, QChar('0'));
    return str.toStdString();
}
