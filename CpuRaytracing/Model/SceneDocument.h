#ifndef SCENEDOCUMENT_H
#define SCENEDOCUMENT_H

#include <string>

#include <CellarWorkbench/DesignPattern/Singleton.h>


class SceneDocument : public cellar::Singleton<SceneDocument>
{
    friend class Singleton< SceneDocument >;

    SceneDocument();
    ~SceneDocument();

public:
    std::string documentRootDirectory() const;
    void setDocumentRootDirectory(const std::string& root);

    std::string sceneName() const;
    void setSceneName(const std::string& name);

    std::string stageSetName() const;
    void setStageSetName(const std::string& name);

    std::string soundtrackName() const;
    void setSoundtrackName(const std::string& name);

    std::string outputFrameDirectory() const;
    void setOutputFrameDirectory(const std::string& directory);

    std::string outputFrameFormat() const;
    void setOutputFrameFormat(const std::string& format);

    bool includeSampleCountInFrame() const;
    void setIncludeSampleCountInFrame(bool include);

    bool includeRenderTimeInFrame() const;
    void setIncludeRenderTimeInFrame(bool include);

    bool includeDivergenceInFrame() const;
    void setIncludeDivergenceInFrame(bool include);

    std::string getStageSetFilePath() const;
    std::string getAnimationPathsFilePath() const;
    std::string getSoundtrackFilePath() const;
    std::string getTextureRootDirectory() const;
    std::string getAnimationFramesDirectory() const;

    static std::string timeToString(double time);

private:
    std::string _rootDir;
    std::string _sceneName;
    std::string _stageSetName;
    std::string _soundtrackName;
    std::string _outputFrameDirectory;
    std::string _outputFrameFormat;
    bool _includeSampleCountInFrame;
    bool _includeRenderTimeInFrame;
    bool _includeDivergenceInFrame;
};

SceneDocument& getSceneDocument();


// IMPLEMENTATION //
inline std::string SceneDocument::documentRootDirectory() const
{
    return _rootDir;
}

inline std::string SceneDocument::sceneName() const
{
    return _sceneName;
}

inline std::string SceneDocument::stageSetName() const
{
    return _stageSetName;
}

inline std::string SceneDocument::soundtrackName() const
{
    return _soundtrackName;
}

inline std::string SceneDocument::outputFrameDirectory() const
{
    return _outputFrameDirectory;
}

inline std::string SceneDocument::outputFrameFormat() const
{
    return _outputFrameFormat;
}

inline bool SceneDocument::includeSampleCountInFrame() const
{
    return _includeSampleCountInFrame;
}

inline bool SceneDocument::includeRenderTimeInFrame() const
{
    return _includeRenderTimeInFrame;
}

inline bool SceneDocument::includeDivergenceInFrame() const
{
    return _includeDivergenceInFrame;
}

#endif // SCENEDOCUMENT_H
