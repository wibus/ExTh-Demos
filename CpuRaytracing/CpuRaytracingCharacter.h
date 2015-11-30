#ifndef CPURAYTRACINGCHARACTER_H
#define CPURAYTRACINGCHARACTER_H

#include <memory>
#include <vector>

#include <GLM/glm.hpp>

#include <Scaena/Play/Character.h>

namespace cellar
{
    class CameraManFree;
}

namespace prop2
{
    class TextHud;
}

namespace prop3
{
    class Prop;
    class Surface;
    class Material;

    class ProceduralSun;
}


class CpuRaytracingCharacter :
        public scaena::Character
{
public:
    CpuRaytracingCharacter();

    virtual void enterStage() override;
    virtual void beginStep(const scaena::StageTime &time) override;
    virtual void draw(const std::shared_ptr<scaena::View> &view,
                      const scaena::StageTime&time) override;
    virtual void exitStage() override;

protected:
    virtual void setupStageStageSet();
    virtual void setupManufacturingStageSet();
    virtual void setupCornBoardStageSet();

    virtual std::shared_ptr<prop3::Surface> createHoleStrippedWall(
            const glm::dvec3& size,
            double stripeWidth,
            double holeWidth,
            double border);

private:
    std::shared_ptr<cellar::CameraManFree> _camMan;

    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;

    std::shared_ptr<prop3::Material> _glassMat;
    std::shared_ptr<prop3::Material> _fixtureMat;
    std::shared_ptr<prop3::Surface> _fixtureSurf;
    std::shared_ptr<prop3::Surface> _fixtureBounds;

    prop3::ProceduralSun* _backdrop;
};

#endif // CPURAYTRACINGCHARACTER_H
