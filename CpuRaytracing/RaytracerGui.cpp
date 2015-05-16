#include "RaytracerGui.h"
#include "ui_RaytracerGui.h"

#include <QSpinBox>

#include "Managers/CameraManager.h"
#include "Managers/PostProdManager.h"
#include "RaytracedView.h"

using namespace scaena;


RaytracerGui::RaytracerGui(const std::shared_ptr<Play>& play) :
    _ui(new Ui::RaytracerGui),
    _play(play)
{
    _ui->setupUi(this);

    _cameraManager.reset(new CameraManager(_ui));
    _postProdManager.reset(new PostProdManager(_ui));
    _raytracedView.reset(new RaytracedView(
         _cameraManager,
         _postProdManager));

    _cameraManager->setView(_raytracedView.get());
    _play->addView(_raytracedView);
}

RaytracerGui::~RaytracerGui()
{
    delete _ui;
}
