#include "QPostProdManager.h"
#include "ui_QPostProdUnit.h"

#include <cassert>

#include <GLM/gtc/constants.hpp>

#include <QShowEvent>
#include <QFileDialog>

#include <CellarWorkbench/Misc/StringUtils.h>
#include <CellarWorkbench/Misc/Log.h>

#include <CellarWorkbench/GL/GlInputsOutputs.h>


QPostProdManager::QPostProdManager() :
    _isSetup(false),
    _ui(new Ui::QPostProdManager())
{
    _ui->setupUi(this);

    connect(_ui->activateLowpassCheck, &QCheckBox::stateChanged,
            this,                      &QPostProdManager::activateLowPassChecked);

    connect(_ui->lowpassSizeCombo, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged,
            this,                  &QPostProdManager::lowpassSizeChanged);

    connect(_ui->lowpassVarianceSpin, (void (QDoubleSpinBox::*)(double)) &QDoubleSpinBox::valueChanged,
            this,                     &QPostProdManager::lowpassVarianceChanged);

    connect(_ui->useAdaptativeFilteringCheck, &QCheckBox::stateChanged,
            this,                             &QPostProdManager::useAdaptativeFilteringChecked);

    connect(_ui->adaptativeFactorSlider, &QSlider::valueChanged,
            this,                        &QPostProdManager::adaptativeFilteringFactorChanged);

    connect(_ui->temperatureSpin,   (void (QSpinBox::*)(int)) &QSpinBox::valueChanged,
            this,                   &QPostProdManager::temperatureChanged);

    connect(_ui->temperatureDefaultBtn, &QPushButton::clicked,
            this,                       &QPostProdManager::temperatureDefaultClicked);

    connect(_ui->contrastSlider, &QSlider::valueChanged,
            this,                &QPostProdManager::contrastChanged);

    connect(_ui->luminositySlider, &QSlider::valueChanged,
            this,                  &QPostProdManager::luminosityChanged);

    connect(_ui->saveButton, &QPushButton::clicked,
            this,            &QPostProdManager::saveOutputImage);
}

QPostProdManager::~QPostProdManager()
{
}

void QPostProdManager::setPostProdUnit(
    std::shared_ptr<prop3::GlPostProdUnit> unitBackend)
{
    _unitBackend = unitBackend;
}

void QPostProdManager::setup()
{
    lowpassSizeChanged(_ui->lowpassSizeCombo->currentIndex());
    lowpassVarianceChanged(_ui->lowpassVarianceSpin->value());
    adaptativeFilteringFactorChanged(_ui->adaptativeFactorSlider->value());
    // LowPassChecked must be done after low pass properties to make sur it
    // overwrites those properties in case low pass is deactivated.
    useAdaptativeFilteringChecked(_ui->useAdaptativeFilteringCheck->checkState());
    activateLowPassChecked(_ui->activateLowpassCheck->checkState());
    temperatureChanged(_ui->temperatureSpin->value());
    temperatureDefaultClicked();
    contrastChanged(_ui->contrastSlider->value());
    luminosityChanged(_ui->luminositySlider->value());

    _isSetup = true;
}

void QPostProdManager::activateLowPassChecked(int state)
{
    bool isChecked = state;
    _ui->lowpassWidget->setEnabled(isChecked);
    _unitBackend->activateLowPassFilter(isChecked);

    if(isChecked)
    {
        updateLowpassKernelTable(_ui->kernelTable, _unitBackend->lowpassKernel());
    }
}

void QPostProdManager::lowpassSizeChanged(int sizeIndex)
{
    prop3::KernelSize size;
    if(sizeIndex == 0) size = prop3::KernelSize::SIZE_3x3;
    else if(sizeIndex == 1) size = prop3::KernelSize::SIZE_5x5;
    else assert(false /* Unsupported kernel size */);

    _unitBackend->setLowpassKernelSize(size);
    updateLowpassKernelTable(_ui->kernelTable, _unitBackend->lowpassKernel());
}

void QPostProdManager::lowpassVarianceChanged(double variance)
{
    _unitBackend->setLowpassVariance(variance);
    updateLowpassKernelTable(_ui->kernelTable, _unitBackend->lowpassKernel());
}

void QPostProdManager::useAdaptativeFilteringChecked(int state)
{
    bool isChecked = state;
    _ui->adaptativeWidget->setEnabled(isChecked);
    _unitBackend->enableAdaptativeFiltering(isChecked);
}

void QPostProdManager::adaptativeFilteringFactorChanged(int factor)
{
    float zeroToOne = computeAdaptativeFactor(factor);
    _ui->adaptativeFactorLabel->setText(QString::number(zeroToOne*100) + '%');
    _unitBackend->setAdaptativeFilteringFactor(zeroToOne);
}

void QPostProdManager::temperatureChanged(int kelvin)
{
    _ui->temperatureDefaultBtn->setEnabled(
        kelvin != prop3::GlPostProdUnit::DEFAULT_WHITE_TEMPERATURE);
    _unitBackend->setImageTemperature(kelvin);
}

void QPostProdManager::temperatureDefaultClicked()
{
    temperatureChanged(prop3::GlPostProdUnit::DEFAULT_WHITE_TEMPERATURE);
}

void QPostProdManager::contrastChanged(int contrast)
{
    float minusOneToOne = computeContrastFactor(contrast);
    _ui->contrastLabel->setText(
        QString::number((int)(minusOneToOne*100)) + '%');
    _unitBackend->setImageContrast(minusOneToOne);
}

void QPostProdManager::luminosityChanged(int luminosity)
{
    float zeroToOne = computeLuminosityFactor(luminosity);
    _ui->luminosityLabel->setText(
        QString::number((double)zeroToOne, 'g', 2));
    _unitBackend->setImageLuminosity(zeroToOne);
}

void QPostProdManager::saveOutputImage()
{
    _unitBackend->saveOutputImage();
}

float QPostProdManager::computeLuminosityFactor(int luminosity)
{
    return (luminosity - 50) / 50.0f;
}

float QPostProdManager::computeContrastFactor(int contrast)
{
    return contrast / 50.0f;
}

float QPostProdManager::computeAdaptativeFactor(int factor)
{
    return factor / 100.0f;
}

void QPostProdManager::updateLowpassKernelTable(QTableWidget* widget, const float* kernel)
{
    for(int j=0; j<5; ++j)
    {
        for(int i=0; i<5; ++i)
        {
            QString cellValue;
            float value = kernel[j*5 + i];
            if(value != 0) cellValue = QString::number(value, 'g', 2);
            widget->setCellWidget(i, j, new QLabel(cellValue));
        }
    }
}
