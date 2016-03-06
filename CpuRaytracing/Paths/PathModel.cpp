#include "PathModel.h"

#include <CellarWorkbench/Misc/Log.h>

#include <CellarWorkbench/Path/LinearPath.h>
#include <CellarWorkbench/Path/PointPath.h>
#include <CellarWorkbench/Path/CompositePath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>

#include <PropRoom3D/Node/StageSet.h>
#include <PropRoom3D/Node/Debug/DebugLineStrip.h>
#include <PropRoom3D/Node/Debug/DebugPointCloud.h>

#include "PathWriter.h"
#include "PathReader.h"

using namespace cellar;
using namespace prop3;


const std::string PathModel::CAMERA_EYE_PATH_NAME   = "Camera Eye";
const std::string PathModel::CAMERA_TO_PATH_NAME    = "Camera To";
const std::string PathModel::CAMERA_FOV_PATH_NAME   = "Camera Fov";
const std::string PathModel::THE_FRUIT_PATH_NAME    = "The Fruit";
const std::string PathModel::CLOUDS_PATH_NAME       = "Clouds";
const std::string PathModel::DAY_TIME_PATH_NAME     = "Day Time";
const std::string PathModel::HALL_LIGHT_PATH_NAME   = "Hall Light";
const std::string PathModel::ROOM_LIGHT_PATH_NAME   = "Room Light";


class CtrlPtDrawer : public PathVisitor<glm::dvec3>
{
public:
    CtrlPtDrawer(const std::shared_ptr<StageSet>& stageSet,
                 const glm::dvec3& outerColor,
                 const glm::dvec3& innerColor) :
        _stageSet(stageSet),
        _innerColor(innerColor),
        _outerColor(outerColor),
        _offset(0, 0, 0)
    {}

    virtual void visit(PointPath<glm::dvec3>& path) override
    {
        DebugPointCloud points(_outerColor);
        points.setVertices({path.value() + _offset});
        _stageSet->addDebugPoints(points);
    }

    virtual void visit(LinearPath<glm::dvec3>& path) override
    {
        DebugPointCloud points(_outerColor);
        points.setVertices({
            path.begin() + _offset,
            path.end() + _offset});
        _stageSet->addDebugPoints(points);
    }

    virtual void visit(CubicSplinePath<glm::dvec3>& path) override
    {
        DebugPointCloud outer(_outerColor);
        outer.setVertices({
            path.ctrlPts().front() + _offset,
            path.ctrlPts().back() + _offset});
        _stageSet->addDebugPoints(outer);

        if(path.ctrlPts().size() > 2)
        {
            DebugPointCloud inner(_innerColor);
            auto end = --path.ctrlPts().end();
            auto it = path.ctrlPts().begin();
            while((it++) != end)
                inner.addVertex(*it  + _offset);
            _stageSet->addDebugPoints(inner);
        }
    }

    virtual void visit(BasisSplinePath<glm::dvec3>& path) override
    {
        DebugPointCloud outer(_outerColor);
        outer.setVertices({
            path.ctrlPts().front() + _offset,
            path.ctrlPts().back() + _offset});
        _stageSet->addDebugPoints(outer);

        if(path.ctrlPts().size() > 2)
        {
            DebugPointCloud inner(_innerColor);
            auto end = --path.ctrlPts().end();
            auto it = path.ctrlPts().begin();
            while((it++) != end)
                inner.addVertex(*it  + _offset);
            _stageSet->addDebugPoints(inner);
        }
    }

    virtual void visit(CompositePath<glm::dvec3>& path) override
    {
        for(auto child : path.paths())
            child->accept(*this);
    }

    void setOffset(const glm::dvec3& offset)
    {
        _offset = offset;
    }

private:
    std::shared_ptr<StageSet> _stageSet;
    glm::dvec3 _outerColor;
    glm::dvec3 _innerColor;
    glm::dvec3 _offset;
};

PathModel::PathModel()
{

}

PathModel::~PathModel()
{

}

void PathModel::init(const std::shared_ptr<StageSet>& stageSet)
{
    _stageSet = stageSet;

    cameraTo.reset(new CompositePath<glm::dvec3>());
    cameraEye.reset(new CompositePath<glm::dvec3>());
    cameraFoV.reset(new CompositePath<double>());
    theFruit.reset(new CompositePath<glm::dvec3>());
    clouds.reset(new CompositePath<glm::dvec3>());
    dayTime.reset(new CompositePath<double>());
    hallLight.reset(new CompositePath<double>());
    roomLight.reset(new CompositePath<double>());
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
    maxLength = glm::max(maxLength, hallLight->duration());
    maxLength = glm::max(maxLength, roomLight->duration());

    return maxLength;
}

void PathModel::refreshDebugLines()
{
    const double DEBUG_FPS = 24.0;

    _stageSet->clearDebugLines();
    _stageSet->clearDebugPoints();

    const glm::dvec3 LINE_COLOR = glm::dvec3(0, 0.7, 0.0);
    const glm::dvec3 INNER_COLOR = glm::dvec3(0.5, 1.0, 0.5);
    const glm::dvec3 OUTER_COLOR = glm::dvec3(0.5, 0.5, 1.0);
    const glm::dvec3 THE_FRUIT_OFFSET = glm::dvec3(0, 0, 0.5);
    CtrlPtDrawer drawer(_stageSet, OUTER_COLOR, INNER_COLOR);


    if(_lineVisibility[CAMERA_EYE_PATH_NAME])
    {
        double duration = cameraEye->duration();
        DebugLineStrip camEyeLine(LINE_COLOR);
        for(double t=0.0; t < duration; t+=1.0/DEBUG_FPS)
            camEyeLine.addVertex(cameraEye->value(t));
        _stageSet->addDebugLine(camEyeLine);

        cameraEye->accept(drawer);
    }

    if(_lineVisibility[CAMERA_TO_PATH_NAME])
    {
        double duration = cameraTo->duration();
        DebugLineStrip camToLine(LINE_COLOR);
        for(double t=0.0; t < duration; t+=1.0/DEBUG_FPS)
            camToLine.addVertex(cameraTo->value(t));
        _stageSet->addDebugLine(camToLine);

        cameraTo->accept(drawer);
    }

    if(_lineVisibility[THE_FRUIT_PATH_NAME])
    {
        double duration = theFruit->duration();
        DebugLineStrip theFruitLine(LINE_COLOR);
        for(double t=0.0; t < duration; t+=1.0/DEBUG_FPS)
            theFruitLine.addVertex(theFruit->value(t) + THE_FRUIT_OFFSET);
        _stageSet->addDebugLine(theFruitLine);

        drawer.setOffset(THE_FRUIT_OFFSET);
        theFruit->accept(drawer);
        drawer.setOffset(glm::dvec3());
    }

    if(_lineVisibility[CLOUDS_PATH_NAME])
    {
        double duration = clouds->duration();
        DebugLineStrip cloudsLine(LINE_COLOR);
        for(double t=0.0; t < duration; t+=1.0/DEBUG_FPS)
            cloudsLine.addVertex(clouds->value(t));
        _stageSet->addDebugLine(cloudsLine);

        clouds->accept(drawer);
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
    pathArray.append( dvec3Writer.write(CLOUDS_PATH_NAME,      *clouds) );
    pathArray.append( doubleWriter.write(DAY_TIME_PATH_NAME,   *dayTime) );
    pathArray.append( doubleWriter.write(HALL_LIGHT_PATH_NAME,  *hallLight) );
    pathArray.append( doubleWriter.write(ROOM_LIGHT_PATH_NAME,  *roomLight) );

    QJsonDocument jsonDoc(pathArray);
    return jsonDoc.toJson(QJsonDocument::Indented).toStdString();
}

bool PathModel::deserialize(const std::string& stream)
{
    PathReader reader;
    if(reader.read(QJsonDocument::fromJson(stream.c_str())))
    {
        if(reader.dvec3Path(CAMERA_EYE_PATH_NAME).get()  != nullptr &&
           reader.dvec3Path(CAMERA_TO_PATH_NAME).get()   != nullptr &&
           reader.doublePath(CAMERA_FOV_PATH_NAME).get() != nullptr &&
           reader.dvec3Path(THE_FRUIT_PATH_NAME).get()   != nullptr &&
           reader.dvec3Path(CLOUDS_PATH_NAME).get()      != nullptr &&
           reader.doublePath(DAY_TIME_PATH_NAME).get()   != nullptr &&
           reader.doublePath(HALL_LIGHT_PATH_NAME).get() != nullptr &&
           reader.doublePath(ROOM_LIGHT_PATH_NAME).get() != nullptr)
        {

            cameraEye = reader.dvec3Path(CAMERA_EYE_PATH_NAME);
            cameraTo = reader.dvec3Path(CAMERA_TO_PATH_NAME);
            cameraFoV = reader.doublePath(CAMERA_FOV_PATH_NAME);
            theFruit = reader.dvec3Path(THE_FRUIT_PATH_NAME);
            clouds = reader.dvec3Path(CLOUDS_PATH_NAME);
            dayTime = reader.doublePath(DAY_TIME_PATH_NAME);
            hallLight = reader.doublePath(HALL_LIGHT_PATH_NAME);
            roomLight = reader.doublePath(ROOM_LIGHT_PATH_NAME);

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
