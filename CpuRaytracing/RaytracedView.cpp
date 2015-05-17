#include "RaytracedView.h"

#include <PropRoom2D/Team/AbstractTeam.h>
#include <PropRoom2D/Team/ArtDirector/GlArtDirector.h>

#include <PropRoom3D/Team/AbstractTeam.h>
#include <PropRoom3D/Team/ArtDirector/CpuRaytracerServer.h>

#include <Scaena/Play/Play.h>

#include "Managers/CameraManager.h"
#include "Managers/PostProdManager.h"


RaytracedView::RaytracedView(
        const std::shared_ptr<CameraManager>& cameraManager,
        const std::shared_ptr<PostProdManager>& postProdManager) :
    scaena::QGlWidgetView("Raytraced View"),
    _cameraManager(cameraManager),
    _postProdManager(postProdManager)
{
}

RaytracedView::~RaytracedView()
{

}

void RaytracedView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void RaytracedView::setupArtDirectors(scaena::Play& play)
{
    _artDirector2D.reset(new prop2::GlArtDirector());
    play.propTeam2D()->addArtDirector(_artDirector2D);
    _artDirector2D->resize(width(), height());

    prop3::CpuRaytracerServer* cpuRaytracer(
        new prop3::CpuRaytracerServer());
    _artDirector3D.reset(cpuRaytracer);
    play.propTeam3D()->addArtDirector(_artDirector3D);
    _artDirector3D->resize(width(), height());

    _cameraManager->setCamera(cpuRaytracer->camera());
    _postProdManager->setPostProdUnit(cpuRaytracer->postProdUnit());
}
