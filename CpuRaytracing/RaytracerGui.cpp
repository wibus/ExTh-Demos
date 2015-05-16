#include "RaytracerGui.h"
#include "ui_RaytracerGui.h"

#include <QSpinBox>

using namespace scaena;


RaytracerGui::RaytracerGui(const std::shared_ptr<Play>& play) :
    _ui(new Ui::RaytracerGui),
    _play(play)
{
    _ui->setupUi(this);

    _postProdManager.reset(new PostProdManager(_ui));

    _raytracedView.reset(new RaytracedView(_postProdManager));
    _ui->raytracedScrollView->setWidget(_raytracedView.get());
    connect(_ui->viewportSpinX,   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            _raytracedView.get(), &RaytracedView::setFixedWidth);
    connect(_ui->viewportSpinY,   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            _raytracedView.get(), &RaytracedView::setFixedHeight);
    _raytracedView->setFixedWidth(_ui->viewportSpinX->value());
    _raytracedView->setFixedHeight(_ui->viewportSpinY->value());

    _play->addView(_raytracedView);
}

RaytracerGui::~RaytracerGui()
{
    delete _ui;
}
