#ifndef VOLUME_RENDERING_VOLUMES_H
#define VOLUME_RENDERING_VOLUMES_H

#include <vector>

#include <GLM/glm.hpp>


class IVolume
{
public:
    virtual glm::vec4 opticalAt(float x, float y, float z, float ds) = 0;
    virtual glm::vec4 materialAt(float x, float y, float z, float ds);

protected:
    virtual void clamp(float& x, float& y, float& z);
    virtual float densityAt(float x, float y, float z, float ds) = 0;
};

class Shell : public IVolume
{
public:
    Shell();
    virtual glm::vec4 opticalAt(float x, float y, float z, float ds) override;
    virtual glm::vec4 materialAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;

private:
    glm::vec3 _center;
};

class Boil : public IVolume
{
public:
    virtual glm::vec4 opticalAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;
};

class SinNoise : public IVolume
{
public:
    virtual glm::vec4 opticalAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;
};

class BallFloor : public IVolume
{
public:
    virtual glm::vec4 opticalAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;
};

#endif //VOLUME_RENDERING_VOLUMES_H
