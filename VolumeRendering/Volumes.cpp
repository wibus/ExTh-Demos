#include "Volumes.h"

#include <Algorithm/Noise.h>
#include <Misc/CellarUtils.h>
using namespace cellar;


void IVolume::clamp(float& x, float& y, float& z)
{
    x = cellar::clamp(x, 0.0f, 1.0f);
    y = cellar::clamp(y, 0.0f, 1.0f);
    z = cellar::clamp(z, 0.0f, 1.0f);
}

cellar::Vec4f IVolume::materialAt(float x, float y, float z, float ds)
{
    return Vec4f(
                Vec3f(
                (densityAt(x+ds, y ,z, ds) - densityAt(x-ds, y ,z, ds)),
                (densityAt(x, y+ds ,z, ds) - densityAt(x, y-ds ,z, ds)),
                (densityAt(x, y ,z+ds, ds) - densityAt(x, y ,z-ds, ds)))
                .normalized(),
            densityAt(x, y ,z, ds));
}


// Shell
Shell::Shell() :
    _center(0.5, 0.5, 0.5)
{

}

Vec4f Shell::opticalAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);
    return Vec4f(x,
                 (y-0.5f)*(y-0.5),
                 z*z,
                 densityAt(x, y, z, ds));
}

cellar::Vec4f Shell::materialAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);
    Vec3f distance = _center - Vec3f(x, y, z);
    return Vec4f(distance.normalized(),
                 densityAt(x, y, z, ds));
}

float Shell::densityAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);
    float w = 0.03f;
    Vec3f pos(x, y, z);
    Vec3f center(0.5f, 0.5f, 0.5f);
    Vec3f distance = pos - center;
    float dist = distance.length();
    float d1 = absolute(dist - 0.45f);
    float d2 = absolute(dist - w);

    float a = (d1>w ? d2>w ? 0.01f : 0.2f : 0.1f);

    return a;
}


// Boil
Vec4f Boil::opticalAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);

    Vec3f pos(x, y, z);
    Vec3f center(0.5f, 0.5f, 0.5f);
    float d = pos.distanceTo(center);

    return Vec4f(x, maxVal(0.0f, 1.0f-d*d), z*z, densityAt(x, y, z, ds));
}

float Boil::densityAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);
    Vec3f pos(x, y, z);
    Vec3f center(0.5f, 0.5f, 0.5f);
    float d = pos.distanceTo(center);

    float f = 4.0f;
    float n = SimplexNoise::noise3d(x*f, y*f, z*f);
    float a = cellar::clamp(n - d*7.0f + 3.0f, 0.0f, 1.0f);
    return a;
}


// SinNoise
Vec4f SinNoise::opticalAt(float x, float y, float z, float ds)
{
    Vec3f pos(x, y, z);
    Vec3f center(0.5f, 0.5f, 0.5f);
    float d = pos.distanceTo(center);
    return Vec4f(x, maxVal(0.0f, 1.0f-d*d), z*z, densityAt(x, y, z, ds));
}

float SinNoise::densityAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);
    Vec3f pos(x, y, z);
    Vec3f center(0.5f, 0.5f, 0.5f);
    float d = pos.distanceTo(center);

    float f = 1.0f;
    float n = SimplexNoise::noise3d(x*f, y*f, z*f);
    float a = absolute(sin(1.0f/n)) * 0.1; // What!!!
    return a;
}


// BallFloor
Vec4f BallFloor::opticalAt(float x, float y, float z, float ds)
{
    return Vec4f(1.0f-x*(x-1.0f), y, z, densityAt(x, y, z, ds));
}

float BallFloor::densityAt(float x, float y, float z, float ds)
{
    clamp(x, y, z);
    float a = 0.0;

    if(z > 0.2)
    {
        Vec3f pos(x, y, z);
        Vec3f center(0.5f, 0.65f, 0.8f);
        float d = pos.distanceTo(center);
        float r = 0.15;
        float o = (r - d)/r;
        a = maxVal(o*o * sign(o), 0.0f);
    }
    else
    {
        a = 1.0f;
    }

    return a;
}
