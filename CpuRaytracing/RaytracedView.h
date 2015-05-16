#ifndef RAYTRACEDVIEW_H
#define RAYTRACEDVIEW_H

#include <Scaena/ScaenaApplication/QGlWidgetView.h>

#include "PostProdManager.h"


class RaytracedView : public scaena::QGlWidgetView
{
public:
    RaytracedView(std::shared_ptr<PostProdManager> postProdManager);
    virtual ~RaytracedView();


protected:
    virtual void setupArtDirectors(scaena::Play& play) override;


private:
    std::shared_ptr<PostProdManager> _postProdManager;
};

#endif // RAYTRACEDVIEW_H
