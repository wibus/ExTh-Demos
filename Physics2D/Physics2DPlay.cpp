#include <memory>
using namespace std;

#include "Physics2DPlay.h"
#include "Physics2DCharacter.h"


Physics2DPlay::Physics2DPlay() :
    SingleActPlay("Physics 2D")
{
}

void Physics2DPlay::loadExternalRessources()
{
}

void Physics2DPlay::setUpPersistentCharacters()
{
    addPersistentCharacter(
        shared_ptr<scaena::AbstractCharacter>(new Physics2DCharacter( stage() )));
}
