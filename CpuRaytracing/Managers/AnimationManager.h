#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <memory>

#include <QWidget>

namespace Ui
{
    class RaytracerGui;
}

class TheFruitChoreographer;
class PathManager;


class AnimationManager : public QObject
{
    Q_OBJECT

public:
    AnimationManager(Ui::RaytracerGui* ui);
    virtual ~AnimationManager();

    virtual void setChoreographer(
        const std::shared_ptr<TheFruitChoreographer>& choreographer);

private slots:
    virtual void animFps(int fps);
    virtual void animFrame(int frame);
    virtual void resetAnim(bool unsused);
    virtual void playAnim(bool play);
    virtual void fastPlay(bool fast);

    virtual void animFrameFromChoreographer(int frame);
    virtual void animPlayFromChoreographer(bool play);

private:


private:
    Ui::RaytracerGui* _ui;
    std::shared_ptr<PathManager> _pathManager;
    std::shared_ptr<TheFruitChoreographer> _choreographer;
};

#endif // ANIMATIONMANAGER_H
