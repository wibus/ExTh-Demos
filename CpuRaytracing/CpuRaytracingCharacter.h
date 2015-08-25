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
    virtual void setupConvergenceStageSet();
    virtual void setupQuadricStageSet();

    virtual void createBusGlass(const glm::dmat4& transform, double width, double height);
    virtual std::shared_ptr<prop3::Prop> createFixture();

private:
    std::shared_ptr<cellar::CameraManFree> _camMan;

    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;
    std::shared_ptr<prop3::Prop> _sphere;
};

#endif // CPURAYTRACINGCHARACTER_H
