#ifndef VOLUME_RENDERING_LIGHTS
#define VOLUME_RENDERING_LIGHTS

#include <GLM/glm.hpp>


class Light
{
public:
    Light(const glm::vec3& position,
          const glm::vec3& color,
          float shininess,
          float ambientContribution,
          bool isCastingShadows);

    glm::vec3 position;
    glm::vec3 color;
    float shininess;
    float ambientContribution;
    bool isCastingShadows;
};

#endif //VOLUME_RENDERING_LIGHTS
