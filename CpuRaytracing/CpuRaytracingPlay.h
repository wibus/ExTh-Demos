#ifndef CPURAYTRACING_PLAY_H
#define CPURAYTRACING_PLAY_H

#include <Play/SingleActPlay.h>


class CpuRaytracingPlay : public scaena::SingleActPlay
{
public:
    CpuRaytracingPlay();

    virtual void loadExternalRessources();
    virtual void setUpPersistentCharacters();
};

#endif // CPURAYTRACING_PLAY_H
