#ifndef THEFRUIT_CHOREOGRAPHER_H
#define THEFRUIT_CHOREOGRAPHER_H

#include <QObject>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/Choreographer/AbstractChoreographer.h>

namespace prop3
{
    class Prop;
    class Surface;
    class ProceduralSun;
    class StageZone;
}

class PathModel;
class PathManager;


struct RecordOutput
{
    std::string name;
    std::string format;
    bool includeSampleCount;
    bool includeRenderTime;
    bool includeDivergence;
};


class TheFruitChoreographer : public QObject, public prop3::AbstractChoreographer
{
    Q_OBJECT

public:
    TheFruitChoreographer(const std::shared_ptr<cellar::Camera>& camera,
                          const std::shared_ptr<prop3::RaytracerState>& raytracerState);
    virtual ~TheFruitChoreographer();


    virtual void setup(const std::shared_ptr<prop3::StageSet>& stageSet) override;
    virtual void update(double dt) override;
    virtual void terminate() override;

    virtual void forceUpdate();

    virtual int animFrameCount();
    virtual void setAnimFps(int fps);
    virtual void setAnimFrame(int frame);
    virtual void resetAnimation();
    virtual void startRecording();
    virtual void stopRecording();
    virtual void playAnimation();
    virtual void pauseAnimation();
    virtual void setFastPlay(bool playFast);

    virtual std::shared_ptr<PathModel> pathModel() const;

    virtual RecordOutput& recordOutput();
    virtual void saveCurrentFrame();

signals:
    void animFrameChanged(int frameId);
    void playStateChanged(bool isPlaying);


protected:
    virtual std::shared_ptr<prop3::Surface> createHoleStrippedWall(
            const glm::dvec3& size,
            double stripeWidth,
            double holeWidth,
            double border);

private:
    std::shared_ptr<cellar::Camera> _camera;
    std::shared_ptr<cellar::CameraManFree> _camMan;
    double _camAperture;

    glm::dvec3 _cloudsPosition;
    std::shared_ptr<prop3::StageZone> _cloudsZone;

    glm::dvec3 _theFruitPosition;
    std::shared_ptr<prop3::Prop> _theFruitProp;
    std::shared_ptr<prop3::ProceduralSun> _backdrop;

    std::shared_ptr<prop3::RaytracerState> _raytracerState;
    std::shared_ptr<PathModel> _pathModel;

    int _animFps;
    int _animFrame;
    double _animTime;
    bool _isRecording;
    bool _animPlaying;
    bool _animFastPlay;
    RecordOutput _recordOutput;
};

#endif // THEFRUIT_CHOREOGRAPHER_H
