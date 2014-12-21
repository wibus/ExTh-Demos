#ifndef VOLUME_RENDERING_PLAY_H
#define VOLUME_RENDERING_PLAY_H

#include <Scaena/Play/SingleActPlay.h>


class VolumeRenderingPlay : public scaena::SingleActPlay
{
public:
    VolumeRenderingPlay();

    virtual void loadExternalRessources();
    virtual void setUpPersistentCharacters();
};

#endif // VOLUME_RENDERING_PLAY_H
