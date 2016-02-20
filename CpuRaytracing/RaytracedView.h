#ifndef RAYTRACEDVIEW_H
#define RAYTRACEDVIEW_H

#include <Scaena/ScaenaApplication/QGlWidgetView.h>


namespace prop3
{
    class ArtDirectorServer;
}

class CameraManager;
class AnimationManager;
class PostProdManager;
class PathManager;

class TheFruitChoreographer;



class RaytracedView : public scaena::QGlWidgetView
{
public:
    RaytracedView(
        const std::shared_ptr<CameraManager>& cameraManager,
        const std::shared_ptr<AnimationManager>& animationManager,
        const std::shared_ptr<PostProdManager>& postProdManager,
        const std::shared_ptr<PathManager>& pathManager);
    virtual ~RaytracedView();


protected:
    virtual void installArtDirectors(scaena::Play& play) override;
    virtual void setup() override;


private:
    std::shared_ptr<CameraManager> _cameraManager;
    std::shared_ptr<AnimationManager> _animationManager;
    std::shared_ptr<PostProdManager> _postProdManager;
    std::shared_ptr<PathManager> _pathManager;

    std::shared_ptr<TheFruitChoreographer> _choreographer;
    std::shared_ptr<prop3::ArtDirectorServer> _raytracerServer;
};

#endif // RAYTRACEDVIEW_H
