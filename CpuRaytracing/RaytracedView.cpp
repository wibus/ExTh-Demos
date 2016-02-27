#include "RaytracedView.h"

#include <PropRoom2D/Team/AbstractTeam.h>
#include <PropRoom2D/Team/ArtDirector/GlArtDirector.h>

#include <PropRoom3D/Team/AbstractTeam.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

#include <Scaena/Play/Play.h>

#include "TheFruitChoreographer.h"
#include "Managers/CameraManager.h"
#include "Managers/AnimationManager.h"
#include "Managers/PostProdManager.h"
#include "Managers/PathManager.h"


RaytracedView::RaytracedView(
        const std::shared_ptr<CameraManager>& cameraManager,
        const std::shared_ptr<AnimationManager>& animationManager,
        const std::shared_ptr<PostProdManager>& postProdManager,
        const std::shared_ptr<PathManager>& pathManager) :
    scaena::QGlWidgetView("Raytraced View"),
    _cameraManager(cameraManager),
    _animationManager(animationManager),
    _postProdManager(postProdManager),
    _pathManager(pathManager)
{
}

RaytracedView::~RaytracedView()
{

}

void RaytracedView::installArtDirectors(scaena::Play& play)
{
    _artDirector2D.reset(new prop2::GlArtDirector());
    play.propTeam2D()->addArtDirector(_artDirector2D);

    _raytracerServer.reset(new prop3::ArtDirectorServer());
    play.propTeam3D()->addArtDirector(_raytracerServer);
    _artDirector3D = _raytracerServer;

    _choreographer.reset(new TheFruitChoreographer(
                _raytracerServer->camera(),
                _raytracerServer->raytracerState()));
    //*
    play.propTeam3D()->switchChoreographer( _choreographer );
    //*/

    _stageSet = play.propTeam3D()->stageSet();
}

void RaytracedView::setup()
{
    _artDirector2D->resize(width(), height());
    _cameraManager->setRaytracer(_raytracerServer);
    _postProdManager->setPostProdUnit(_raytracerServer->postProdUnit());
    _animationManager->setChoreographer(_choreographer);
    _animationManager->setRaytracer(_raytracerServer);

    _pathManager->setStageSet(_stageSet);
    _pathManager->setChoreographer(_choreographer);
}
