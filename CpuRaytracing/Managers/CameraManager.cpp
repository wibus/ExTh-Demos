#include "CameraManager.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <QKeyEvent>

#include "ui_RaytracerGui.h"


CameraManager::CameraManager(Ui::RaytracerGui* ui) :
    _ui(ui),
    _fullscreenWindow(nullptr)
{
    _ui->fieldOfViewSlider->setMinimum(_ui->fieldOfViewSpin->minimum());
    _ui->fieldOfViewSlider->setMaximum(_ui->fieldOfViewSpin->maximum());
    _ui->fieldOfViewSlider->setValue(_ui->fieldOfViewSpin->value());
}

CameraManager::~CameraManager()
{    
    _ui->raytracedScrollView->setWidget(_view);
    delete _fullscreenWindow;
}

void CameraManager::setView(QWidget* view)
{
    _view = view;

    _ui->raytracedScrollView->setWidget(_view);

    _fullscreenWindow = new QWidget();
    _fullscreenWindow->installEventFilter(this);
    QLayout* fullscreenLayout = new QHBoxLayout();
    fullscreenLayout->setAlignment(Qt::AlignCenter);
    _fullscreenWindow->setLayout(fullscreenLayout);
    QPalette background(QPalette::Background, Qt::black);
    _fullscreenWindow->setPalette(background);

    connect(_ui->customOutputDimensionsRadioBtn, &QRadioButton::toggled,
            _ui->customOutputDimensionsWidget,   &QWidget::setEnabled);

    connect(_ui->matchCaptureSizeRadioBtn, &QRadioButton::toggled,
            this, &CameraManager::outputMatchCaptureDimensions);

    connect(_ui->captureDimensionsSpinX,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::captureDimensionsChanged);

    connect(_ui->captureDimensionsSpinY,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::captureDimensionsChanged);

    connect(_ui->outputDimensionsSpinX,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::outputDimensionsChanged);

    connect(_ui->outputDimensionsSpinY,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::outputDimensionsChanged);

    connect(_ui->fieldOfViewSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            _ui->fieldOfViewSpin,   &QSpinBox::setValue);

    connect(_ui->fieldOfViewSpin,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            _ui->fieldOfViewSlider, &QSlider::setValue);

    connect(_ui->fieldOfViewSpin,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::fieldOfViewChanged);

    connect(_ui->fullscreenButton, &QPushButton::clicked,
            this,                  &CameraManager::enterFullscreen);

    outputDimensionsChanged(0);
    outputMatchCaptureDimensions(_ui->matchCaptureSizeRadioBtn->isChecked());
}

void CameraManager::setCamera(const std::shared_ptr<cellar::Camera>& camera)
{
    _camera = camera;
    captureDimensionsChanged(0);
}

void CameraManager::captureDimensionsChanged(int)
{
    _camera->updateViewport(
        _ui->captureDimensionsSpinX->value(),
        _ui->captureDimensionsSpinY->value());
    updateCameraProjection();
}

void CameraManager::outputDimensionsChanged(int)
{
    _view->setFixedSize(
        _ui->outputDimensionsSpinX->value(),
        _ui->outputDimensionsSpinY->value());
}

void CameraManager::outputMatchCaptureDimensions(bool match)
{
    if(match)
    {
        connect(_ui->captureDimensionsSpinX,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                _ui->outputDimensionsSpinX, &QSpinBox::setValue);

        connect(_ui->captureDimensionsSpinY,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                _ui->outputDimensionsSpinY, &QSpinBox::setValue);

        _ui->outputDimensionsSpinX->setValue(
            _ui->captureDimensionsSpinX->value());
        _ui->outputDimensionsSpinY->setValue(
            _ui->captureDimensionsSpinY->value());
    }
    else
    {
        disconnect(_ui->captureDimensionsSpinX,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   _ui->outputDimensionsSpinX, &QSpinBox::setValue);

        disconnect(_ui->captureDimensionsSpinY,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   _ui->outputDimensionsSpinY, &QSpinBox::setValue);
    }
}

void CameraManager::fieldOfViewChanged(int)
{
    updateCameraProjection();
}


void CameraManager::enterFullscreen()
{
    _ui->raytracedScrollView->takeWidget();

    _fullscreenWindow->layout()->addWidget(_view);
    _fullscreenWindow->showFullScreen();
    _fullscreenWindow->setFocus();
}

void CameraManager::exitFullscreen()
{
    _fullscreenWindow->hide();
    _fullscreenWindow->layout()->takeAt(0);

    _ui->raytracedScrollView->setWidget(_view);
}

void CameraManager::updateCameraProjection()
{
    glm::mat4 projection =
        glm::perspectiveFov(
            glm::radians((float)_ui->fieldOfViewSpin->value())/2,
            (float) _ui->captureDimensionsSpinX->value(),
            (float) _ui->captureDimensionsSpinY->value(),
            1.0f, 2.0f);

    _camera->updateProjection(projection);
}

bool CameraManager::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == _fullscreenWindow)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Escape)
            {
                exitFullscreen();
                return true;
            }
        }

        return false;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}
