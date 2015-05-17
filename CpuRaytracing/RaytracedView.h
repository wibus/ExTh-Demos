#ifndef RAYTRACEDVIEW_H
#define RAYTRACEDVIEW_H

#include <Scaena/ScaenaApplication/QGlWidgetView.h>

class CameraManager;
class PostProdManager;


class RaytracedView : public scaena::QGlWidgetView
{
public:
    RaytracedView(
        const std::shared_ptr<CameraManager>& cameraManager,
        const std::shared_ptr<PostProdManager>& postProdManager);
    virtual ~RaytracedView();


protected:
    virtual void resizeGL(int w, int h) override;
    virtual void setupArtDirectors(scaena::Play& play) override;


private:
    std::shared_ptr<CameraManager> _cameraManager;
    std::shared_ptr<PostProdManager> _postProdManager;
};

#endif // RAYTRACEDVIEW_H
