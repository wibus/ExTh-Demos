#ifndef RAYTRACEDVIEW_H
#define RAYTRACEDVIEW_H

#include <Scaena/ScaenaApplication/QGlWidgetView.h>

class CameraManager;
class PostProdManager;

namespace prop3
{
    class ArtDirectorServer;
}


class RaytracedView : public scaena::QGlWidgetView
{
public:
    RaytracedView(
        const std::shared_ptr<CameraManager>& cameraManager,
        const std::shared_ptr<PostProdManager>& postProdManager);
    virtual ~RaytracedView();


protected:
    virtual void installArtDirectors(scaena::Play& play) override;
    virtual void setup() override;


private:
    std::shared_ptr<CameraManager> _cameraManager;
    std::shared_ptr<PostProdManager> _postProdManager;
    std::shared_ptr<prop3::ArtDirectorServer> _raytracerServer;
};

#endif // RAYTRACEDVIEW_H
