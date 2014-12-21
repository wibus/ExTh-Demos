#ifndef VOLUME_RENDERING_LIGHTS
#define VOLUME_RENDERING_LIGHTS

#include <CellarWorkbench/DataStructure/Vector.h>


class Light
{
public:
    Light(const cellar::Vec3f& position,
          const cellar::Vec3f& color,
          float shininess,
          float ambientContribution,
          bool isCastingShadows);

    cellar::Vec3f position;
    cellar::Vec3f color;
    float shininess;
    float ambientContribution;
    bool isCastingShadows;
};

#endif //VOLUME_RENDERING_LIGHTS
