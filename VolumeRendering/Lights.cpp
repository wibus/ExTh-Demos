#include "Lights.h"

Light::Light(const cellar::Vec3f& position,
             const cellar::Vec3f& color,
             float shininess,
             float ambientContribution,
             bool isCastingShadows) :
    position(position),
    color(color),
    shininess(shininess),
    ambientContribution(ambientContribution),
    isCastingShadows(isCastingShadows)
{

}
