#include "PathModel.h"

#include <CellarWorkbench/Misc/Log.h>

#include <CellarWorkbench/Path/CompositePath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/LinearPath.h>

#include <PropRoom3D/Node/StageSet.h>
#include <PropRoom3D/Node/Debug/DebugLine.h>

#include "PathWriter.h"
#include "PathReader.h"

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

    CompositePath<glm::dvec3>* cameraToComposite = new CompositePath<glm::dvec3>();
    cameraToComposite->addPath(std::make_shared<LinearPath<glm::dvec3>>(1.0, glm::dvec3(), glm::dvec3()));
    cameraTo.reset(cameraToComposite);

    CompositePath<glm::dvec3>* cameraEyeComposite = new CompositePath<glm::dvec3>();
    cameraEyeComposite->addPath(std::make_shared<LinearPath<glm::dvec3>>(1.0, glm::dvec3(), glm::dvec3()));
    cameraEye.reset(cameraEyeComposite);

    CompositePath<double>* cameraFoVComposite = new CompositePath<double>();
    cameraFoVComposite->addPath(std::make_shared<LinearPath<double>>(1.0, 0.0, 0.0));
    cameraFoV.reset(cameraFoVComposite);

    CompositePath<glm::dvec3>* theFruitComposite = new CompositePath<glm::dvec3>();
    theFruitComposite->addPath(std::make_shared<LinearPath<glm::dvec3>>(1.0, glm::dvec3(), glm::dvec3()));
    theFruit.reset(theFruitComposite);

    CompositePath<glm::dvec3>* cloudsComposite = new CompositePath<glm::dvec3>();
    cloudsComposite->addPath(std::make_shared<LinearPath<glm::dvec3>>(1.0, glm::dvec3(), glm::dvec3()));
    clouds.reset(cloudsComposite);

    CompositePath<double>* dayTimeComposite = new CompositePath<double>();
    dayTimeComposite->addPath(std::make_shared<LinearPath<double>>(1.0, 0.0, 0.0));
    dayTime.reset(dayTimeComposite);
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

std::string PathModel::serialize() const
{
    PathWriter<double> doubleWriter;
    PathWriter<glm::dvec3> dvec3Writer;

    QJsonArray pathArray;
    pathArray.append( dvec3Writer.write(CAMERA_EYE_PATH_NAME,  *cameraEye) );
    pathArray.append( dvec3Writer.write(CAMERA_TO_PATH_NAME,   *cameraTo) );
    pathArray.append( doubleWriter.write(CAMERA_FOV_PATH_NAME, *cameraFoV) );
    pathArray.append( dvec3Writer.write(THE_FRUIT_PATH_NAME,   *theFruit) );
    pathArray.append( doubleWriter.write(DAY_TIME_PATH_NAME,   *dayTime) );
    pathArray.append( dvec3Writer.write(CLOUDS_PATH_NAME,      *clouds) );

    QJsonDocument jsonDoc(pathArray);
    return jsonDoc.toJson(QJsonDocument::Indented).toStdString();
}

bool PathModel::deserialize(const std::string& stream)
{
    PathReader reader;
    if(reader.read(QJsonDocument::fromJson(stream.c_str())))
    {
        if(reader.dvec3Path(CAMERA_EYE_PATH_NAME).get() != nullptr &&
           reader.dvec3Path(CAMERA_TO_PATH_NAME).get() != nullptr &&
           reader.doublePath(CAMERA_FOV_PATH_NAME).get() != nullptr &&
           reader.dvec3Path(THE_FRUIT_PATH_NAME).get() != nullptr &&
           reader.doublePath(DAY_TIME_PATH_NAME).get() != nullptr &&
           reader.dvec3Path(CLOUDS_PATH_NAME).get() != nullptr)
        {

            cameraEye = reader.dvec3Path(CAMERA_EYE_PATH_NAME);
            cameraTo = reader.dvec3Path(CAMERA_TO_PATH_NAME);
            cameraFoV = reader.doublePath(CAMERA_FOV_PATH_NAME);
            theFruit = reader.dvec3Path(THE_FRUIT_PATH_NAME);
            dayTime = reader.doublePath(DAY_TIME_PATH_NAME);
            clouds = reader.dvec3Path(CLOUDS_PATH_NAME);

            refreshDebugLines();

            return true;
        }
        else
        {
            getLog().postMessage(new Message('E', false,
                "One of the path is missing...", "PathModel"));
        }
    }
    else
    {
        getLog().postMessage(new Message('E', false,
            "JSon stream couln't be parsed", "PathModel"));
    }

    getLog().postMessage(new Message('W', false,
        "Path model didn't update", "PathModel"));

    return false;
}
