#ifndef PHYSICS2D_PLAY_H
#define PHYSICS2D_PLAY_H

#include <Scaena/Play/SingleActPlay.h>


class Physics2DPlay : public scaena::SingleActPlay
{
public:
    Physics2DPlay();

    virtual void loadExternalRessources();
    virtual void setUpPersistentCharacters();
};

#endif // PHYSICS2D_PLAY_H