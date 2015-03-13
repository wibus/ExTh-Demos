#include "Lights.h"

Light::Light(const glm::vec3& position,
             const glm::vec3& color,
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
