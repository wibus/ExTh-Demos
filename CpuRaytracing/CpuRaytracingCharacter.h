#ifndef CPURAYTRACING_CHARACTER_H
#define CPURAYTRACING_CHARACTER_H

#include <memory>

#include <GLM/glm.hpp>

#include <Scaena/Character/AbstractCharacter.h>


namespace prop2
{
    class TextHud;
}

namespace prop3
{
    class Prop;
}

class CpuRaytracingCharacter : public scaena::AbstractCharacter
{
public:
    CpuRaytracingCharacter(scaena::AbstractStage& stage);

    virtual void enterStage();
    virtual void beginStep(const scaena::StageTime &time);
    virtual void draw(const scaena::StageTime &time);
    virtual void exitStage();

    virtual bool keyPressEvent(const scaena::KeyboardEvent& event);
    virtual bool keyReleaseEvent(const scaena::KeyboardEvent& event);
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

#endif // CPURAYTRACING_CHARACTER_H
