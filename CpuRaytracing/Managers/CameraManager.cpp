#include "CameraManager.h"

#include "ui_RaytracerGui.h"


CameraManager::CameraManager(Ui::RaytracerGui* ui) :
    _ui(ui)
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::setView(QWidget* view)
{
    _view = view;

    _ui->raytracedScrollView->setWidget(_view);
    _view->setFixedWidth(_ui->outputDimensionsSpinX->value());
    _view->setFixedHeight(_ui->outputDimensionsSpinY->value());

    connect(_ui->customDimensionsRadioBtn, &QRadioButton::toggled,
            _ui->customDimensionsFrame,    &QFrame::setEnabled);

    connect(_ui->customOutputDimensionsRadioBtn, &QRadioButton::toggled,
            _ui->customOutputDimensionsWidget,   &QWidget::setEnabled);

    connect(_ui->outputDimensionsSpinX,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            _view, &QWidget::setFixedWidth);

    connect(_ui->outputDimensionsSpinY,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            _view, &QWidget::setFixedHeight);
}

void CameraManager::setCamera(const std::shared_ptr<cellar::Camera>& camera)
{
    _camera = camera;
}

