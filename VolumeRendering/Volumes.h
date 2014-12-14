#ifndef VOLUME_RENDERING_VOLUMES_H
#define VOLUME_RENDERING_VOLUMES_H

#include <vector>
#include <DataStructure/Vector.h>


class IVolume
{
public:
    virtual cellar::Vec4f opticalAt(float x, float y, float z, float ds) = 0;
    virtual cellar::Vec4f materialAt(float x, float y, float z, float ds);

protected:
    virtual void clamp(float& x, float& y, float& z);
    virtual float densityAt(float x, float y, float z, float ds) = 0;
};

class Shell : public IVolume
{
public:
    Shell();
    virtual cellar::Vec4f opticalAt(float x, float y, float z, float ds) override;
    virtual cellar::Vec4f materialAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;

private:
    cellar::Vec4f _center;
};

class Boil : public IVolume
{
public:
    virtual cellar::Vec4f opticalAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;
};

class SinNoise : public IVolume
{
public:
    virtual cellar::Vec4f opticalAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;
};

class BallFloor : public IVolume
{
public:
    virtual cellar::Vec4f opticalAt(float x, float y, float z, float ds) override;

protected:
    virtual float densityAt(float x, float y, float z, float ds) override;
};

#endif //VOLUME_RENDERING_VOLUMES_H
