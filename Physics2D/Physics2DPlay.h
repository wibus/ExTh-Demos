#ifndef PLAY_H
#define PLAY_H

#include <Play/SingleActPlay.h>


class Physics2DPlay : public scaena::SingleActPlay
{
public:
    Physics2DPlay();

    virtual void loadExternalRessources();
    virtual void setUpPersistentCharacters();
};

#endif // PLAY_H
