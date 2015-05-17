#ifndef CPURAYTRACINGCHARACTER_H
#define CPURAYTRACINGCHARACTER_H

#include <memory>

#include <GLM/glm.hpp>

#include <Scaena/Play/Character.h>

namespace cellar
{
    class CameraMsg;
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
    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;
    std::shared_ptr<prop3::Prop> _walls;
    std::shared_ptr<prop3::Prop> _glassLens;
    std::shared_ptr<prop3::Prop> _chromeBall;

    glm::dvec3 _upVec;
    glm::dvec3 _frontVec;
    glm::dvec3 _rightVec;
    glm::dvec3 _camPos;

};

#endif // CPURAYTRACINGCHARACTER_H
