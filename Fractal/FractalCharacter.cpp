#include "FractalCharacter.h"

#include <iostream>

#include <Scaena/Play/Play.h>
#include <Scaena/StageManagement/Event/KeyboardEvent.h>
#include <Scaena/StageManagement/Event/SynchronousKeyboard.h>

using namespace std;
using namespace cellar;
using namespace scaena;


FractalsCharacter::FractalsCharacter() :
    Character("Fractal Chracter"),
    _update(true),
    _fractalProgram(),
    _fractalsVao(),
    _center(0.0, 0.0),
    _scale(2.5),
    _nbIter(1)
{
}

void FractalsCharacter::enterStage()
{
    GlInputsOutputs inout;
    inout.setInput(0, "position");
    inout.setOutput(0, "FragColor");
    _fractalProgram.setInAndOutLocations(inout);
    _fractalProgram.addShader(GL_VERTEX_SHADER, ":/fractals.vert");
    _fractalProgram.addShader(GL_FRAGMENT_SHADER, ":/fractals.frag");
    _fractalProgram.link();
    _fractalProgram.pushProgram();
    _fractalProgram.setFloat("Scale", _scale);
    _fractalProgram.setVec2f("Center", _center);
    _fractalProgram.setInt("MaxIterations", _nbIter);
    _fractalProgram.setVec4f("LowOut", glm::vec4(0.0, 0.0, 0.5, 1.0));
    _fractalProgram.setVec4f("HighOut", glm::vec4(1.0, 1.0, 0.0, 1.0));
    _fractalProgram.popProgram();

    GlVbo2Df positions;
    positions.attribLocation = _fractalProgram.getAttributeLocation("position");
    positions.dataArray.push_back(glm::vec2(-1.0, -1.0));
    positions.dataArray.push_back(glm::vec2(1.0, -1.0));
    positions.dataArray.push_back(glm::vec2(1.0, 1.0));
    positions.dataArray.push_back(glm::vec2(-1.0, 1.0));
    _fractalsVao.createBuffer("position", positions);
}

void FractalsCharacter::beginStep(const scaena::StageTime &time)
{
    float speed = 1/30.0f;
    float scaleFact = 11.0f / 10.0f;

    if(play().synchronousKeyboard()->isAsciiPressed('A'))
    {
        _center += glm::vec2(-1.0, 0.0)*_scale * speed;
    }
    if(play().synchronousKeyboard()->isAsciiPressed('D'))
    {
        _center += glm::vec2(1.0, 0.0)*_scale * speed;
    }
    if(play().synchronousKeyboard()->isAsciiPressed('S'))
    {
        _center += glm::vec2(0.0, -1.0)*_scale * speed;
    }
    if(play().synchronousKeyboard()->isAsciiPressed('W'))
    {
        _center += glm::vec2(0.0, 1.0)*_scale * speed;
    }


    if(play().synchronousKeyboard()->isNonAsciiPressed(ENonAscii::UP))
    {
        _scale /= scaleFact;
    }
    if(play().synchronousKeyboard()->isNonAsciiPressed(ENonAscii::DOWN))
    {
        _scale *= scaleFact;
    }
}

void FractalsCharacter::draw(const std::shared_ptr<scaena::View>&,
                             const scaena::StageTime& time)
{
    _fractalProgram.pushProgram();
    _fractalProgram.setFloat("Scale", _scale);
    _fractalProgram.setVec2f("Center", _center);
    _fractalProgram.setInt("MaxIterations", _nbIter);

    _fractalsVao.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    _fractalsVao.unbind();
    _fractalProgram.popProgram();
}

void FractalsCharacter::exitStage()
{
}

bool FractalsCharacter::keyPressEvent(const scaena::KeyboardEvent &event)
{
    if(play().synchronousKeyboard()->isAsciiPressed('+'))
    {
        if(_nbIter < 2048)
        {
            _nbIter += log(_nbIter*4);
        }
    }
    if(play().synchronousKeyboard()->isAsciiPressed('-'))
    {
        if(_nbIter > 1)
        {
            _nbIter -= log(_nbIter);
        }
    }
    return true;
}
