#ifndef CPURAYTRACINGCHARACTER_H
#define CPURAYTRACINGCHARACTER_H

#include <memory>

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


private:
    std::shared_ptr<cellar::CameraManFree> _camMan;

    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;
    std::shared_ptr<prop3::Prop> _socle;
    std::shared_ptr<prop3::Prop> _stage;
    std::shared_ptr<prop3::Prop> _bowl;
    std::shared_ptr<prop3::Prop> _ball;
};

#endif // CPURAYTRACINGCHARACTER_H
