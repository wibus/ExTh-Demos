#include "AnimationManager.h"

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>

#include "PathManager.h"
#include "../Model/PathModel.h"
#include "../Model/SceneDocument.h"
#include "../TheFruitChoreographer.h"

#include "ui_RaytracerGui.h"


AnimationManager::AnimationManager(Ui::RaytracerGui* ui) :
    _ui(ui)
{
    connect(_ui->maxSampleSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::maxSampleThreshold);

    connect(_ui->maxTimeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::maxRenderTimeThreshold);

    connect(_ui->divThresholdSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &AnimationManager::divThreshold);


    connect(_ui->animTimeOffsetMinSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &AnimationManager::animTimeOffset);
    connect(_ui->animTimeOffsetSecSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &AnimationManager::animTimeOffset);

    connect(_ui->animFpsSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::animFps);

    connect(_ui->animFrameSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::animFrame);

    connect(_ui->animFrameSlider, &QSlider::valueChanged,
            this, &AnimationManager::animFrame);

    connect(_ui->resetAnimButton, &QPushButton::clicked,
            this, &AnimationManager::resetAnim);

    connect(_ui->recordAnimButton, &QPushButton::toggled,
            this, &AnimationManager::recordAnim);

    connect(_ui->playAnimButton, &QPushButton::toggled,
            this, &AnimationManager::playAnim);

    connect(_ui->fastAnimButton, &QPushButton::toggled,
            this, &AnimationManager::fastPlay);

    _ui->animFrameSlider->setValue(
        _ui->animFrameSpin->value());


    _ui->outputFormatCombo->addItem(".png");
    _ui->outputFormatCombo->addItem(".jpg");

    connect(_ui->animNameEdit, &QLineEdit::textChanged,
            this, &AnimationManager::outputName);

    connect(_ui->outputFormatCombo, &QComboBox::currentTextChanged,
            this, &AnimationManager::outputFormat);

    connect(_ui->animNameSampleCountCheck, &QCheckBox::toggled,
            this, &AnimationManager::includeSampleCount);

    connect(_ui->animNameRenderTimeCheck, &QCheckBox::toggled,
            this, &AnimationManager::includeRenderTime);

    connect(_ui->animNameDivergenceCheck, &QCheckBox::toggled,
            this, &AnimationManager::includeDivergence);
}

AnimationManager::~AnimationManager()
{

}

void AnimationManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;

    animTimeOffset(0 /*unused*/);
    animFps(_ui->animFpsSpin->value());
    animFrame(_ui->animFrameSpin->value());
    fastPlay(_ui->fastAnimButton->isChecked());
    playAnim(_ui->playAnimButton->isChecked());

    connect(_choreographer.get(), &TheFruitChoreographer::animFrameChanged,
            this, &AnimationManager::animFrameFromChoreographer);

    connect(_choreographer.get(), &TheFruitChoreographer::playStateChanged,
            this, &AnimationManager::animPlayFromChoreographer);
}

void AnimationManager::setRaytracer(
    const std::shared_ptr<prop3::ArtDirectorServer>& raytracer)
{
    _raytracer = raytracer;

    maxSampleThreshold(_ui->maxSampleSpin->value());
    maxRenderTimeThreshold(_ui->maxTimeSpin->value());
    divThreshold(_ui->divThresholdSpin->value());

    outputName(_ui->animNameEdit->text());
    outputFormat(_ui->outputFormatCombo->currentText());
    includeSampleCount(_ui->animNameSampleCountCheck->isChecked());
    includeRenderTime(_ui->animNameRenderTimeCheck->isChecked());
    includeDivergence(_ui->animNameDivergenceCheck->isChecked());
}

void AnimationManager::maxSampleThreshold(int sampleCount)
{
    _raytracer->raytracerState()->setSampleCountThreshold(sampleCount);
}

void AnimationManager::maxRenderTimeThreshold(int maxSeconds)
{
    _raytracer->raytracerState()->setRenderTimeThreshold(maxSeconds);
}

void AnimationManager::divThreshold(double div)
{
    _raytracer->raytracerState()->setDivergenceThreshold(div);
}

void AnimationManager::animTimeOffset(double offset)
{
    _choreographer->setAnimTimeOffset(
        computeTimeOffset());
    updateTimeMeter();
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
    updateTimeMeter();
}

void AnimationManager::resetAnim(bool unsused)
{
    _choreographer->resetAnimation();
}

void AnimationManager::recordAnim(bool record)
{
    if(record)
        _choreographer->startRecording();
    else
        _choreographer->stopRecording();

    _ui->playAnimButton->setChecked(record);
    _ui->animOutputGroup->setEnabled(!record);
}

void AnimationManager::playAnim(bool play)
{
    if(play)
        _choreographer->playAnimation();
    else
    {
        _choreographer->pauseAnimation();
        _ui->recordAnimButton->setChecked(false);
    }
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
    updateTimeMeter();
}

void AnimationManager::animPlayFromChoreographer(bool play)
{
    _ui->playAnimButton->setChecked(play);
}

void AnimationManager::outputName(const QString& name)
{
    getSceneDocument().setOutputFrameDirectory(name.toStdString());
}

void AnimationManager::outputFormat(const QString& format)
{
    getSceneDocument().setOutputFrameFormat(format.toStdString());
}

void AnimationManager::includeSampleCount(bool include)
{
    getSceneDocument().setIncludeSampleCountInFrame(include);
}

void AnimationManager::includeRenderTime(bool include)
{
    getSceneDocument().setIncludeRenderTimeInFrame(include);
}

void AnimationManager::includeDivergence(bool include)
{
    getSceneDocument().setIncludeDivergenceInFrame(include);
}

void AnimationManager::updateTimeMeter()
{
    double offset = computeTimeOffset();
    int frame = _ui->animFrameSpin->value();
    _ui->animTimeValue->setText(
        SceneDocument::timeToString(offset + double(frame) / _ui->animFpsSpin->value()).c_str());
    _ui->animTimeSuffix->setText(QString(" / %1")
        .arg(SceneDocument::timeToString(offset + _choreographer->pathModel()->animationLength()).c_str()));
}

double AnimationManager::computeTimeOffset()
{
    double offsetMin = _ui->animTimeOffsetMinSpin->value();
    double offsetSec = _ui->animTimeOffsetSecSpin->value();
    double offsetSecTot = offsetMin * 60.0 + offsetSec;
    return offsetSecTot;
}

void AnimationManager::onPathChanged()
{
    int frameCount = _choreographer->animFrameCount();
    _ui->animFrameSpin->setMaximum(frameCount);
    _ui->animFrameSpin->setSuffix(QString("/ %1").arg(frameCount));
    _ui->animFrameSlider->setMaximum(frameCount);
    updateTimeMeter();
}
