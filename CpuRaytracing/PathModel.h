#ifndef PATHMODEL_H
#define PATHMODEL_H

#include <map>
#include <memory>

#include <CellarWorkbench/Path/AbstractPath.h>

namespace prop3
{
    class StageSet;
}


class PathModel
{
public:
    PathModel();
    virtual ~PathModel();

    void init(const std::shared_ptr<prop3::StageSet>& stageSet);

    double animationLength() const;


    void refreshDebugLines();

    bool isDebugLineVisible(const std::string& name);

    void setDebugLineVisibility(const std::string& name, bool isVisible);


    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> cameraEye;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> cameraTo;
    std::shared_ptr<cellar::AbstractPath<double>> cameraFoV;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> theFruit;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> clouds;
    std::shared_ptr<cellar::AbstractPath<double>> dayTime;

    static const std::string CAMERA_EYE_PATH_NAME;
    static const std::string CAMERA_TO_PATH_NAME;
    static const std::string CAMERA_FOV_PATH_NAME;
    static const std::string THE_FRUIT_PATH_NAME;
    static const std::string DAY_TIME_PATH_NAME;
    static const std::string CLOUDS_PATH_NAME;

private:
    std::shared_ptr<prop3::StageSet> _stageSet;
    std::map<std::string, bool> _lineVisibility;
};

#endif // PATHMODEL_H
