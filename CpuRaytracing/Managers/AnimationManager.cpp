#include "AnimationManager.h"

#include <QFileInfo>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/ArtDirector/Film/Film.h>

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


    connect(_ui->animTimeOffsetMinSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
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

    connect(_ui->soundtrackNameEdit, &QLineEdit::textChanged,
            this, &AnimationManager::soundtrackName);

    connect(_ui->soundtrackVolumeSlider, &QSlider::valueChanged,
            this, &AnimationManager::soundtrackVolume);

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


    connect(_ui->saveReferenceShotButton, &QPushButton::clicked,
            this, &AnimationManager::saveReferenceShot);

    connect(_ui->loadReferenceShotButton, &QPushButton::clicked,
            this, &AnimationManager::loadReferenceShot);

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
    soundtrackName(_ui->soundtrackNameEdit->text());
    soundtrackVolume(_ui->soundtrackVolumeSlider->value());

    connect(_choreographer.get(), &TheFruitChoreographer::animFrameChanged,
            this, &AnimationManager::animFrameFromChoreographer);

    connect(_choreographer.get(), &TheFruitChoreographer::playStateChanged,
            this, &AnimationManager::animPlayFromChoreographer);

    connect(_choreographer.get(), &TheFruitChoreographer::rawFilmSourceChanged,
            this, &AnimationManager::rawFilmSourceChanged);

    connect(_choreographer.get(), &TheFruitChoreographer::saveRawFilm,
            this, &AnimationManager::saveRawFilm);
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

void AnimationManager::animTimeOffset(double)
{
    double offset = computeTimeOffset();
    getSceneDocument().setAnimationTimeOffset(offset);
    _choreographer->setAnimTimeOffset(offset);
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

    if(_ui->playAnimButton->isChecked() &&
       _ui->fastAnimButton->isChecked())
    {
        startSoundtrack();
    }
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
    {
        _choreographer->playAnimation();
        if(_ui->fastAnimButton->isChecked())
            startSoundtrack();
    }
    else
    {
        _choreographer->pauseAnimation();
        _ui->recordAnimButton->setChecked(false);
        _ui->fastAnimButton->setChecked(false);
    }
}

void AnimationManager::fastPlay(bool fast)
{
    _choreographer->setFastPlay(fast);

    _ui->animFrameSpin->setEnabled(!fast);
    _ui->animFrameSlider->setEnabled(!fast);
    _ui->soundtrackNameEdit->setEnabled(!fast);

    if(fast && _ui->playAnimButton->isChecked())
        startSoundtrack();
    else
        _mediaPlayer.pause();
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

void AnimationManager::soundtrackName(QString name)
{
    getSceneDocument().setSoundtrackName(name.toStdString());
    std::string relative = getSceneDocument().getSoundtrackFilePath();
    QString absolute = QFileInfo(relative.c_str()).absoluteFilePath();
    _mediaPlayer.setMedia(QUrl::fromLocalFile(absolute));
}

void AnimationManager::soundtrackVolume(int volume)
{
    _mediaPlayer.setVolume(volume);
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

void AnimationManager::saveReferenceShot()
{
    _raytracer->film()->saveReferenceShot(
        getSceneDocument().getAnimationFilmsDirectory() + "/reference.film");
}

void AnimationManager::loadReferenceShot()
{
    _raytracer->film()->loadReferenceShot(
        getSceneDocument().getAnimationFilmsDirectory() + "/reference.film");
}

void AnimationManager::rawFilmSourceChanged()
{
    _raytracer->film()->setRawFilmSource(
        _choreographer->currentFilm());
}

void AnimationManager::saveRawFilm()
{
    _raytracer->film()->saveRawFilm(
        _choreographer->currentFilm());
}

void AnimationManager::startSoundtrack()
{
    double time = computeCurrentTime();
    qint64 pos = time * 1000.0;
    _mediaPlayer.setPosition(pos);
    _mediaPlayer.play();
}

void AnimationManager::updateTimeMeter()
{
    double current = computeCurrentTime();
    double offset = computeTimeOffset();
    double endTime = offset + _choreographer->pathModel()->animationLength();

    _ui->animTimeValue->setText(
        SceneDocument::timeToString(current).c_str());
    _ui->animTimeSuffix->setText(QString(" / %1")
        .arg(SceneDocument::timeToString(endTime).c_str()));
}

double AnimationManager::computeTimeOffset()
{
    double offsetMin = _ui->animTimeOffsetMinSpin->value();
    double offsetSec = _ui->animTimeOffsetSecSpin->value();
    double offsetSecTot = offsetMin * 60.0 + offsetSec;
    return offsetSecTot;
}

double AnimationManager::computeCurrentTime()
{
    double offset = computeTimeOffset();
    double frame = _ui->animFrameSpin->value();
    double fps = _ui->animFpsSpin->value();
    return offset + frame/fps;
}

void AnimationManager::onPathChanged()
{
    int frameCount = _choreographer->animFrameCount();
    _ui->animFrameSpin->setMaximum(frameCount);
    _ui->animFrameSpin->setSuffix(QString("/ %1").arg(frameCount));
    _ui->animFrameSlider->setMaximum(frameCount);
    updateTimeMeter();
}
