#include <memory>
using namespace std;

#include "VolumeRenderingPlay.h"
#include "Visualizer.h"


VolumeRenderingPlay::VolumeRenderingPlay() :
    SingleActPlay("Volume Rendering")
{
}

void VolumeRenderingPlay::loadExternalRessources()
{
}

void VolumeRenderingPlay::setUpPersistentCharacters()
{
    addPersistentCharacter(
        shared_ptr<scaena::AbstractCharacter>(
            new Visualizer( stage() )));
}
