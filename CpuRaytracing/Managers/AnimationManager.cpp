#include "AnimationManager.h"

#include "../TheFruitChoreographer.h"

#include "ui_RaytracerGui.h"


AnimationManager::AnimationManager(Ui::RaytracerGui* ui) :
    _ui(ui)
{
    connect(_ui->animFpsSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::animFps);

    connect(_ui->animFrameSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::animFrame);

    connect(_ui->animFrameSlider, &QSlider::valueChanged,
            this, &AnimationManager::animFrame);

    connect(_ui->resetAnimButton, &QPushButton::clicked,
            this, &AnimationManager::resetAnim);

    connect(_ui->playAnimButton, &QPushButton::toggled,
            this, &AnimationManager::playAnim);

    connect(_ui->fastAnimButton, &QPushButton::toggled,
            this, &AnimationManager::fastPlay);

    _ui->animFrameSlider->setValue(
        _ui->animFrameSpin->value());
}

AnimationManager::~AnimationManager()
{

}

void AnimationManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;

    animFps(_ui->animFpsSpin->value());
    animFrame(_ui->animFrameSpin->value());
    fastPlay(_ui->fastAnimButton->isChecked());
    playAnim(_ui->playAnimButton->isChecked());

    connect(_choreographer.get(), &TheFruitChoreographer::animFrameChanged,
            this, &AnimationManager::animFrameFromChoreographer);

    connect(_choreographer.get(), &TheFruitChoreographer::playStateChanged,
            this, &AnimationManager::animPlayFromChoreographer);
}

void AnimationManager::animFps(int fps)
{
    _choreographer->setAnimFps(fps);

    int frameCount = _choreographer->animFrameCount();
    _ui->animFrameSpin->setMaximum(frameCount);
    _ui->animFrameSlider->setMaximum(frameCount);
}

void AnimationManager::animFrame(int frame)
{
    _choreographer->setAnimFrame(frame);
    _ui->animFrameSpin->setValue(frame);
    _ui->animFrameSlider->setValue(frame);
}

void AnimationManager::resetAnim(bool unsused)
{
    _choreographer->resetAnimation();
}

void AnimationManager::playAnim(bool play)
{
    if(play)
        _choreographer->playAnimation();
    else
        _choreographer->pauseAnimation();
}

void AnimationManager::fastPlay(bool fast)
{
    _choreographer->setFastPlay(fast);

    _ui->animFrameSpin->setEnabled(!fast);
    _ui->animFrameSlider->setEnabled(!fast);
}

void AnimationManager::animFrameFromChoreographer(int frame)
{
    _ui->animFrameSpin->setValue(frame);
    _ui->animFrameSlider->setValue(frame);
}

void AnimationManager::animPlayFromChoreographer(bool play)
{
    _ui->playAnimButton->setChecked(play);
}
