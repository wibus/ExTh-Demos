#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <memory>

#include <QWidget>
#include <QMediaPlayer>

#include <CellarWorkbench/Camera/Camera.h>

#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>


namespace Ui
{
    class RaytracerGui;
}

class TheFruitChoreographer;


class AnimationManager : public QObject
{
    Q_OBJECT

public:
    AnimationManager(Ui::RaytracerGui* ui);
    virtual ~AnimationManager();

    virtual void setChoreographer(
        const std::shared_ptr<TheFruitChoreographer>& choreographer);
    virtual void setRaytracer(
            const std::shared_ptr<prop3::ArtDirectorServer>& raytracer);


public slots:
    virtual void onPathChanged();


private slots:
    virtual void divThreshold(double div);
    virtual void maxSampleThreshold(int sampleCount);
    virtual void maxRenderTimeThreshold(int maxSeconds);

    virtual void animTimeOffset(double offset);
    virtual void animFps(int fps);
    virtual void animFrame(int frame);
    virtual void resetAnim(bool unsused);
    virtual void recordAnim(bool record);
    virtual void playAnim(bool play);
    virtual void fastPlay(bool fast);

    virtual void animFrameFromChoreographer(int frame);
    virtual void animPlayFromChoreographer(bool play);

    virtual void soundtrackName(QString name);
    virtual void soundtrackVolume(int volume);

    virtual void outputName(const QString& name);
    virtual void outputFormat(const QString& format);
    virtual void includeSampleCount(bool include);
    virtual void includeRenderTime(bool include);
    virtual void includeDivergence(bool include);

    virtual void saveReferenceShot();
    virtual void loadReferenceShot();
    virtual void rawFilmSourceChanged();
    virtual void saveRawFilm();

protected:
    virtual void startSoundtrack();
    virtual void updateTimeMeter();
    virtual double computeTimeOffset();
    virtual double computeCurrentTime();

private:
    Ui::RaytracerGui* _ui;
    std::shared_ptr<prop3::ArtDirectorServer> _raytracer;
    std::shared_ptr<TheFruitChoreographer> _choreographer;
    QMediaPlayer _mediaPlayer;

};

#endif // ANIMATIONMANAGER_H
