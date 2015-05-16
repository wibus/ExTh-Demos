#ifndef FRACTALS_CHARACTER
#define FRACTALS_CHARACTER

#include <memory>

#include <CellarWorkbench/GL/GlProgram.h>
#include <CellarWorkbench/GL/GlVao.h>

#include <Scaena/Play/Character.h>


class FractalsCharacter : public scaena::Character
{
public:
    FractalsCharacter();

    void enterStage() override;
    void beginStep(const scaena::StageTime &time) override;
    void draw(const std::shared_ptr<scaena::View> &view,
              const scaena::StageTime &time) override;
    void exitStage() override;

    bool keyPressEvent(const scaena::KeyboardEvent &event) override;


private:
    bool              _update;
    cellar::GlProgram _fractalProgram;
    cellar::GlVao     _fractalsVao;
    glm::vec2         _center;
    float             _scale;
    int               _nbIter;
};

#endif //FRACTALS_CHARACTER
