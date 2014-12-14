#ifndef CPURAYTRACING_CHARACTER_H
#define CPURAYTRACING_CHARACTER_H

#include <memory>

#include <Character/AbstractCharacter.h>


namespace prop2
{
    class TextHud;
}

class CpuRaytracingCharacter : public scaena::AbstractCharacter
{
public:
    CpuRaytracingCharacter(scaena::AbstractStage& stage);

    virtual void enterStage();
    virtual void beginStep(const scaena::StageTime &time);
    virtual void draw(const scaena::StageTime &time);
    virtual void exitStage();

private:
    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;
};

#endif // CPURAYTRACING_CHARACTER_H
