#include <memory>
using namespace std;

#include "CpuRaytracingPlay.h"
#include "CpuRaytracingCharacter.h"


CpuRaytracingPlay::CpuRaytracingPlay() :
    SingleActPlay("Cpu Raytracing")
{
}

void CpuRaytracingPlay::loadExternalRessources()
{
}

void CpuRaytracingPlay::setUpPersistentCharacters()
{
    addPersistentCharacter(
        shared_ptr<scaena::AbstractCharacter>(
            new CpuRaytracingCharacter( stage() )));
}
