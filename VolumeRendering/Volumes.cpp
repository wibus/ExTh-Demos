#include "Volumes.h"

#include <CellarWorkbench/Misc/SimplexNoise.h>

using namespace cellar;


void IVolume::clamp(float& x, float& y, float& z)
{
    x = glm::clamp(x, 0.0f, 1.0f);
    y = glm::clamp(y, 0.0f, 1.0f);
    z = glm::clamp(z, 0.0f, 1.0f);
}

glm::vec4 IVolume::materialAt(float x, float y, float z, float ds)
{
    return glm::vec4(
                glm::normalize(
                    glm::vec3(
            (densityAt(x+ds, y ,z, ds) - densityAt(x-ds, y ,z, ds)),
            (densityAt(x, y+ds ,z, ds) - densityAt(x, y-ds ,z, ds)),
            (densityAt(x, y ,z+ds, ds) - densityAt(x, y ,z-ds, ds)))),
            densityAt(x, y ,z, ds));
}


// Shell
Shell::Shell() :
    _center(0.5, 0.5, 0.5)
{

}

glm::vec4 Shell::opticalAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);
    return glm::vec4(x,
                 (y-0.5f)*(y-0.5),
                 z*z,
                 densityAt(x, y, z, ds));
}

glm::vec4 Shell::materialAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);
    glm::vec3 distance = _center - glm::vec3(x, y, z);
    return glm::vec4(glm::normalize(distance),
                 densityAt(x, y, z, ds));
}

float Shell::densityAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);
    float w = 0.03f;
    glm::vec3 pos(x, y, z);
    glm::vec3 center(0.5f, 0.5f, 0.5f);
    glm::vec3 distance = pos - center;
    float dist = distance.length();
    float d1 = glm::abs(dist - 0.45f);
    float d2 = glm::abs(dist - w);

    float a = (d1>w ? d2>w ? 0.01f : 0.2f : 0.1f);

    return a;
}


// Boil
glm::vec4 Boil::opticalAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);

    glm::vec3 pos(x, y, z);
    glm::vec3 center(0.5f, 0.5f, 0.5f);
    float d = glm::length(pos - center);

    return glm::vec4(x, glm::max(0.0f, 1.0f-d*d), z*z, densityAt(x, y, z, ds));
}

float Boil::densityAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);
    glm::vec3 pos(x, y, z);
    glm::vec3 center(0.5f, 0.5f, 0.5f);
    float d = glm::length(pos - center);

    float f = 4.0f;
    float n = SimplexNoise::noise3d(x*f, y*f, z*f);
    float a = glm::clamp(n - d*7.0f + 3.0f, 0.0f, 1.0f);
    return a;
}


// SinNoise
glm::vec4 SinNoise::opticalAt(float x, float y, float z, float ds)
{
    glm::vec3 pos(x, y, z);
    glm::vec3 center(0.5f, 0.5f, 0.5f);
    float d = glm::length(pos - center);
    return glm::vec4(x, glm::max(0.0f, 1.0f-d*d), z*z, densityAt(x, y, z, ds));
}

float SinNoise::densityAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);
    glm::vec3 pos(x, y, z);
    glm::vec3 center(0.5f, 0.5f, 0.5f);
    float d = glm::length(pos - center);

    float f = 1.0f;
    float n = SimplexNoise::noise3d(x*f, y*f, z*f);
    float a = glm::abs(sin(1.0f/n)) * 0.1; // What!!!
    return a;
}


// BallFloor
glm::vec4 BallFloor::opticalAt(float x, float y, float z, float ds)
{
    return glm::vec4(1.0f-x*(x-1.0f), y, z, densityAt(x, y, z, ds));
}

float BallFloor::densityAt(float x, float y, float z, float ds)
{
    glm::clamp(x, y, z);
    float a = 0.0;

    if(z > 0.2)
    {
        glm::vec3 pos(x, y, z);
        glm::vec3 center(0.5f, 0.65f, 0.8f);
        float d = glm::length(pos - center);
        float r = 0.15;
        float o = (r - d)/r;
        a = glm::max(o*o * glm::sign(o), 0.0f);
    }
    else
    {
        a = 1.0f;
    }

    return a;
}
