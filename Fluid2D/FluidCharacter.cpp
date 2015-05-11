#include "FluidCharacter.h"

#include <cmath>
#include <iostream>
#include <memory>

#include <GLM/gtc/matrix_transform.hpp>

#include <GL3/gl3w.h>

#include <CellarWorkbench/Misc/StringUtils.h>
#include <CellarWorkbench/Misc/SimplexNoise.h>

#include <PropRoom2D/Team/AbstractTeam.h>

#include <Scaena/Play/Play.h>
#include <Scaena/Play/View.h>
#include <Scaena/StageManagement/Event/StageTime.h>
#include <Scaena/StageManagement/Event/KeyboardEvent.h>
#include <Scaena/StageManagement/Event/MouseEvent.h>
#include <Scaena/StageManagement/Event/SynchronousKeyboard.h>
#include <Scaena/StageManagement/Event/SynchronousMouse.h>

using namespace std;
using namespace cellar;
using namespace prop2;
using namespace scaena;


const int FluidCharacter::WIDTH = 256;
const int FluidCharacter::HEIGHT = 256;
const int FluidCharacter::AREA = WIDTH * HEIGHT;
const int FluidCharacter::POINT_SIZE = 3;


FluidCharacter::FluidCharacter(Play& play) :
    Character(play, "FluidCharacter"),
    DX(1.0f),
    DT(1.0f),
    VISCOSITY(0.01f),
    HEATDIFF(0.01f),
    _drawShader(),
    _vao(),
    DRAW_TEX(1),
    FETCH_TEX(0),
    _statsPanel(),
    _fps(),
    _ups()
{
}

FluidCharacter::~FluidCharacter()
{

}

void FluidCharacter::enterStage()
{
    // GL resources
    GlVbo2Df buffPos;
    buffPos.attribLocation = 0;
    buffPos.dataArray.push_back(glm::vec2(-1.0, -1.0));
    buffPos.dataArray.push_back(glm::vec2( 1.0, -1.0));
    buffPos.dataArray.push_back(glm::vec2( 1.0,  1.0));
    buffPos.dataArray.push_back(glm::vec2(-1.0,  1.0));

    _vao.createBuffer("position", buffPos);


    GlInputsOutputs updateLocations;
    updateLocations.setInput(buffPos.attribLocation, "position");
    updateLocations.setOutput(0, "FragOut");

    _advectShader.setInAndOutLocations(updateLocations);
    _advectShader.addShader(GL_VERTEX_SHADER, ":/shaders/update.vert");
    _advectShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/advect.frag");
    _advectShader.link();
    _advectShader.pushProgram();
    _advectShader.setInt("FragInTex", 0);
    _advectShader.setInt("VelocityTex", 1);
    _advectShader.setInt("FrontierTex", 2);
    _advectShader.setVec2f("Size", glm::vec2(WIDTH, HEIGHT));
    _advectShader.setFloat("rDx", 1.0f / DX);
    _advectShader.setFloat("Dt",  DT);
    _advectShader.popProgram();


    _jacobiShader.setInAndOutLocations(updateLocations);
    _jacobiShader.addShader(GL_VERTEX_SHADER, ":/shaders/update.vert");
    _jacobiShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/jacobi.frag");
    _jacobiShader.link();
    _jacobiShader.pushProgram();
    _jacobiShader.setInt("XTex", 0);
    _jacobiShader.setInt("BTex", 1);
    _jacobiShader.setVec2f("Size", glm::vec2(WIDTH, HEIGHT));
    _jacobiShader.setFloat("Alpha", 1);
    _jacobiShader.setFloat("rBeta", 1);
    _jacobiShader.popProgram();

    _divergenceShader.setInAndOutLocations(updateLocations);
    _divergenceShader.addShader(GL_VERTEX_SHADER, ":/shaders/update.vert");
    _divergenceShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/divergence.frag");
    _divergenceShader.link();
    _divergenceShader.pushProgram();
    _divergenceShader.setInt("VelocityTex", 0);
    _divergenceShader.setVec2f("Size", glm::vec2(WIDTH, HEIGHT));
    _divergenceShader.setFloat("HalfrDx", 0.5f / DX);
    _divergenceShader.popProgram();


    _gradSubShader.setInAndOutLocations(updateLocations);
    _gradSubShader.addShader(GL_VERTEX_SHADER, ":/shaders/update.vert");
    _gradSubShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/gradSub.frag");
    _gradSubShader.link();
    _gradSubShader.pushProgram();
    _gradSubShader.setInt("PressureTex", 0);
    _gradSubShader.setInt("VelocityTex", 1);
    _gradSubShader.setVec2f("Size", glm::vec2(WIDTH, HEIGHT));
    _gradSubShader.setFloat("HalfrDx", 0.5f / DX);
    _gradSubShader.popProgram();


    GlInputsOutputs heatLocations;
    heatLocations.setInput(buffPos.attribLocation, "position");
    heatLocations.setOutput(0, "Velocity");
    heatLocations.setOutput(1, "Heat");
    _heatShader.setInAndOutLocations(heatLocations);
    _heatShader.addShader(GL_VERTEX_SHADER, ":/shaders/update.vert");
    _heatShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/heat.frag");
    _heatShader.link();
    _heatShader.pushProgram();
    _heatShader.setInt("VelocityTex", 0);
    _heatShader.setInt("HeatTex", 1);
    _heatShader.setVec2f("MousePos", -glm::vec2(WIDTH, HEIGHT));
    _heatShader.setVec2f("Size", glm::vec2(WIDTH, HEIGHT));
    _heatShader.setFloat("HalfrDx", 0.5f / DX);
    _heatShader.popProgram();


    GlInputsOutputs frontierLocations;
    frontierLocations.setInput(buffPos.attribLocation, "position");
    frontierLocations.setOutput(0, "Velocity");
    frontierLocations.setOutput(1, "Pressure");
    _frontierShader.setInAndOutLocations(frontierLocations);
    _frontierShader.addShader(GL_VERTEX_SHADER, ":/shaders/update.vert");
    _frontierShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/frontier.frag");
    _frontierShader.link();
    _frontierShader.pushProgram();
    _frontierShader.setInt("VelocityTex", 0);
    _frontierShader.setInt("PressureTex", 1);
    _frontierShader.setInt("FrontierTex", 2);
    _frontierShader.setVec2f("Size", glm::vec2(WIDTH, HEIGHT));
    _frontierShader.popProgram();


    GlInputsOutputs drawLocations;
    drawLocations.setInput(buffPos.attribLocation, "position");
    drawLocations.setOutput(0, "FragColor");
    _drawShader.setInAndOutLocations(drawLocations);
    _drawShader.addShader(GL_VERTEX_SHADER, ":/shaders/drawFluid.vert");
    _drawShader.addShader(GL_FRAGMENT_SHADER, ":/shaders/drawFluid.frag");
    _drawShader.link();
    _drawShader.pushProgram();
    _drawShader.setInt("DyeTex",      0);
    _drawShader.setInt("VelocityTex", 1);
    _drawShader.setInt("PressureTex", 2);
    _drawShader.setInt("HeatTex",     3);
    _drawShader.popProgram();
    // End GL resources


    // Stats Panel
    _statsPanel = play().propTeam2D()->createImageHud();
    _statsPanel->setSize(glm::vec2(128, 64));
    _statsPanel->setImageName(":/textures/statsPanel.bmp");
    _statsPanel->setHandlePosition(glm::vec2(6.0, -70.0));
    _statsPanel->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _statsPanel->setVerticalAnchor(EVerticalAnchor::TOP);

    _fps = play().propTeam2D()->createTextHud();
    _fps->setColor(glm::vec4(0/255.0, 3/255.0, 80/255.0, 1.0));
    _fps->setHeight(20);
    _fps->setHandlePosition(_statsPanel->handlePosition() + glm::dvec2(50, 33));
    _fps->setHorizontalAnchor(_statsPanel->horizontalAnchor());
    _fps->setVerticalAnchor(_statsPanel->verticalAnchor());

    _ups = play().propTeam2D()->createTextHud();
    _ups->setColor(_fps->color());
    _ups->setHeight(20);
    _ups->setHandlePosition(_statsPanel->handlePosition() + glm::dvec2(50, 9));
    _ups->setHorizontalAnchor(_statsPanel->horizontalAnchor());
    _ups->setVerticalAnchor(_statsPanel->verticalAnchor());
    // End Stats Panel


    // OpenGL states
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glGenTextures(2, _dyeTex);
    glGenTextures(2, _velocityTex);
    glGenTextures(2, _pressureTex);
    glGenTextures(2, _heatTex);
    glGenTextures(1, &_frontierTex);
    glGenTextures(1, &_tempDivTex);

    typedef float texComp_t;
    typedef glm::tvec4<texComp_t> texVec_t;
    vector<texVec_t> dyeImg(AREA);
    vector<texVec_t> velocityImg(AREA);
    vector<texVec_t> pressureImg(AREA);
    vector<texVec_t> heatImg(AREA);
    vector<texVec_t> frontierImg(AREA);
    for(int i=0; i<AREA; ++i)
    {
        float s = (i%WIDTH)/(float)WIDTH;
        float t = (i/WIDTH)/(float)HEIGHT;
        dyeImg[i]      = initDye(s, t);
        velocityImg[i] = initVelocity(s, t);
        pressureImg[i] = initPressure(s, t);
        heatImg[i]     = initHeat(s, t);
        frontierImg[i] = initFrontier(s, t);
    }

    initTexture(_dyeTex[0],      dyeImg);
    initTexture(_dyeTex[1],      dyeImg);
    initTexture(_velocityTex[0], velocityImg);
    initTexture(_velocityTex[1], velocityImg);
    initTexture(_pressureTex[0], pressureImg);
    initTexture(_pressureTex[1], pressureImg);
    initTexture(_heatTex[0],     heatImg);
    initTexture(_heatTex[1],     heatImg);
    initTexture(_frontierTex,    frontierImg);
    initTexture(_tempDivTex,     vector<texVec_t>(AREA));

    _dyeAtt[DRAW_TEX]  = GL_COLOR_ATTACHMENT0;
    _dyeAtt[FETCH_TEX] = GL_COLOR_ATTACHMENT1;
    _velocityAtt[DRAW_TEX]  = GL_COLOR_ATTACHMENT2;
    _velocityAtt[FETCH_TEX] = GL_COLOR_ATTACHMENT3;
    _pressureAtt[DRAW_TEX]  = GL_COLOR_ATTACHMENT4;
    _pressureAtt[FETCH_TEX] = GL_COLOR_ATTACHMENT5;
    _heatAtt[DRAW_TEX]  = GL_COLOR_ATTACHMENT6;
    _heatAtt[FETCH_TEX] = GL_COLOR_ATTACHMENT7;


    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _dyeAtt[DRAW_TEX],
                           GL_TEXTURE_2D,  _dyeTex[DRAW_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _dyeAtt[FETCH_TEX],
                           GL_TEXTURE_2D,  _dyeTex[FETCH_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _velocityAtt[DRAW_TEX],
                           GL_TEXTURE_2D,  _velocityTex[DRAW_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _velocityAtt[FETCH_TEX],
                           GL_TEXTURE_2D,  _velocityTex[FETCH_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _pressureAtt[DRAW_TEX],
                           GL_TEXTURE_2D,  _pressureTex[DRAW_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _pressureAtt[FETCH_TEX],
                           GL_TEXTURE_2D,  _pressureTex[FETCH_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _heatAtt[DRAW_TEX],
                           GL_TEXTURE_2D,  _heatTex[DRAW_TEX], 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, _heatAtt[FETCH_TEX],
                           GL_TEXTURE_2D,  _heatTex[FETCH_TEX], 0);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // End OpenGL states


    play().view()->camera2D()->registerObserver(*this);
}

glm::vec4 FluidCharacter::initDye(float s, float t)
{
    float zoom = 4.0f;
    float dye = SimplexNoise::noise2d(s*zoom, t*zoom);
    return glm::vec4(dye, dye, dye, 1.0);
}

glm::vec4 FluidCharacter::initVelocity(float s, float t)
{
    /* Swirl
    const float cx = 0.5f, cy = 0.5f;
    const float ed = 0.35f;
    float dist = glm::vec2(s, t).distanceTo(cx, cy);
    if(dist < ed)
        return glm::vec4(t-cx, -(s-cy), 0, 0) * 3.0 + glm::vec4(1.0, 1.0, 0, 0) * 1.0;
    return glm::vec4();
    //*/

    /* Plank
    if(cellar::inRange(s, 0.3f, 0.5f) &&
       cellar::inRange(t, 0.2f, 0.40f))
    {
        return glm::vec4(0.0, 1.0, 0.0, 0.0) * (0.1-glm::abs(t-0.3f))*10.0;
    }
    if(cellar::inRange(s, 0.5f, 0.7f) &&
       cellar::inRange(t, 0.6f, 0.8f))
    {
        return glm::vec4(0.0, -1.0, 0.0, 0.0) * (0.1-glm::abs(t-0.7f))*10.0;
    }
    return glm::vec4();
    //*/

    return glm::vec4();
}

glm::vec4 FluidCharacter::initPressure(float s, float t)
{
    return glm::vec4();
}

glm::vec4 FluidCharacter::initHeat(float s, float t)
{
    if(glm::length(glm::vec2(s, t) - glm::vec2(0.2, 0.8))< 0.08)
        return glm::vec4(-5, 0, 0, 0);
    if(glm::length(glm::vec2(s, t) - glm::vec2(0.5, 0.2)) < 0.08)
        return glm::vec4(5, 0, 0, 0);
    return glm::vec4();
}

glm::vec4 FluidCharacter::initFrontier(float s, float t)
{
    const glm::vec4 block(1.0, 1.0, 1.0, 1.0);
    const glm::vec4 fluid(0.0, 0.0, 0.0, 0.0);

    const float W = 0.03;
    if(s < W || s > 1-W || t < W || t > 1-W)
        return block;

    if((t > 0.45 && t < 0.52) && (
        (s < 0.22f || s > 0.24f) &&
        (s < 0.50f || s > 0.53f) &&
        (s < 0.78f || s > 0.80f)))
        return block;
/*
    if(glm::vec2(s, t).distanceTo(0.75, 0.66) < 0.2)
        return block;

    if(glm::vec2(s, t).distanceTo(0.3, 0.2) < 0.03)
        return block;
*/
    return fluid;
}

template<typename T>
void FluidCharacter::initTexture(unsigned int texId, const T& img)
{
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0,
                 GL_RGBA, GL_FLOAT, img.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FluidCharacter::beginStep(const scaena::StageTime &time)
{
}

void FluidCharacter::endStep(const scaena::StageTime &time)
{
    _ups->setText(toString(time.framesPerSecond()));
}

void FluidCharacter::draw(const std::shared_ptr<scaena::View> &,
                          const scaena::StageTime &time)
{
    _fps->setText(toString(time.framesPerSecond()));

    _vao.bind();
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, WIDTH, HEIGHT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    advect();
    diffuse();
    heat();
    computePressure();
    substractPressureGradient();
    frontier();

    glm::ivec2 viewport = play().view()->viewport();
    glViewport(0, 0, viewport.x, viewport.y);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    drawFluid();

    glEnable(GL_DEPTH_TEST);
    _vao.unbind();

    //exit(0);
}

void FluidCharacter::advect()
{
    _advectShader.pushProgram();

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _frontierTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE0);

    // Dye
    glBindTexture(GL_TEXTURE_2D, _dyeTex[FETCH_TEX]);
    glDrawBuffer(_dyeAtt[DRAW_TEX]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    swap(_dyeTex[FETCH_TEX], _dyeTex[DRAW_TEX]);
    swap(_dyeAtt[FETCH_TEX], _dyeAtt[DRAW_TEX]);

    // Heat
    glBindTexture(GL_TEXTURE_2D, _heatTex[FETCH_TEX]);
    glDrawBuffer(_heatAtt[DRAW_TEX]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    swap(_heatTex[FETCH_TEX], _heatTex[DRAW_TEX]);
    swap(_heatAtt[FETCH_TEX], _heatAtt[DRAW_TEX]);

    // Velocity
    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);
    glDrawBuffer(_velocityAtt[DRAW_TEX]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    swap(_velocityTex[FETCH_TEX], _velocityTex[DRAW_TEX]);
    swap(_velocityAtt[FETCH_TEX], _velocityAtt[DRAW_TEX]);

    _advectShader.popProgram();
}

void FluidCharacter::diffuse()
{
    const int NB_ITERATIONS = 60;

    _jacobiShader.pushProgram();

    // Velocity
    _jacobiShader.setFloat("Alpha", DX*DX / (VISCOSITY*DT));
    _jacobiShader.setFloat("rBeta", 1.0f / (4.0f + DX*DX/(VISCOSITY*DT)) );

    for(int i=0; i < (NB_ITERATIONS/2)*2; ++i)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);

        glDrawBuffer(_velocityAtt[DRAW_TEX]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Swap textures
        swap(_velocityTex[FETCH_TEX], _velocityTex[DRAW_TEX]);
        swap(_velocityAtt[FETCH_TEX], _velocityAtt[DRAW_TEX]);
    }

    // Heat
    _jacobiShader.setFloat("Alpha", DX*DX / (HEATDIFF*DT));
    _jacobiShader.setFloat("rBeta", 1.0f / (4.0f + DX*DX/(HEATDIFF*DT)) );
    for(int i=0; i < (NB_ITERATIONS/2)*2; ++i)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _heatTex[FETCH_TEX]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _heatTex[FETCH_TEX]);

        glDrawBuffer(_heatAtt[DRAW_TEX]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Swap textures
        swap(_heatTex[FETCH_TEX], _heatTex[DRAW_TEX]);
        swap(_heatAtt[FETCH_TEX], _heatAtt[DRAW_TEX]);
    }


    _jacobiShader.popProgram();
}

void FluidCharacter::heat()
{
    const GLenum drawBuffers [] = {
        _velocityAtt[DRAW_TEX],
        _heatAtt[DRAW_TEX],
    };

    _heatShader.pushProgram();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _heatTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);

    glDrawBuffers(2, drawBuffers);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    swap(_heatTex[FETCH_TEX],     _heatTex[DRAW_TEX]);
    swap(_heatAtt[FETCH_TEX],     _heatAtt[DRAW_TEX]);
    swap(_velocityTex[FETCH_TEX], _velocityTex[DRAW_TEX]);
    swap(_velocityAtt[FETCH_TEX], _velocityAtt[DRAW_TEX]);

    _heatShader.popProgram();
}

void FluidCharacter::computePressure()
{
    _divergenceShader.pushProgram();

    // Manque de color attachement oblige...
    glFramebufferTexture2D(GL_FRAMEBUFFER, _heatAtt[DRAW_TEX],
                           GL_TEXTURE_2D,  _tempDivTex, 0);

    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);
    glDrawBuffer(_heatAtt[DRAW_TEX]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Et on remet comme c'etait :)
    glFramebufferTexture2D(GL_FRAMEBUFFER, _heatAtt[DRAW_TEX],
                           GL_TEXTURE_2D,  _heatTex[DRAW_TEX], 0);

    _divergenceShader.popProgram();


    _jacobiShader.pushProgram();
    _jacobiShader.setFloat("Alpha", -DX*DX);
    _jacobiShader.setFloat("rBeta", 1.0f / 4.0f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _tempDivTex);
    glActiveTexture(GL_TEXTURE0);

    const int NB_ITERATIONS = 60;
    for(int i=0; i < (NB_ITERATIONS/2)*2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, _pressureTex[FETCH_TEX]);

        glDrawBuffer(_pressureAtt[DRAW_TEX]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Swap textures
        swap(_pressureTex[FETCH_TEX], _pressureTex[DRAW_TEX]);
        swap(_pressureAtt[FETCH_TEX], _pressureAtt[DRAW_TEX]);
    }

    _jacobiShader.popProgram();
}

void FluidCharacter::substractPressureGradient()
{
    _gradSubShader.pushProgram();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _pressureTex[FETCH_TEX]);

    glDrawBuffer(_velocityAtt[DRAW_TEX]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    swap(_velocityTex[FETCH_TEX], _velocityTex[DRAW_TEX]);
    swap(_velocityAtt[FETCH_TEX], _velocityAtt[DRAW_TEX]);

    _gradSubShader.popProgram();
}

void FluidCharacter::frontier()
{
    const GLenum drawBuffers [] = {
        _velocityAtt[DRAW_TEX],
        _pressureAtt[DRAW_TEX],
    };

    _frontierShader.pushProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _frontierTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _pressureTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);

    glDrawBuffers(2, drawBuffers);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    swap(_velocityTex[FETCH_TEX], _velocityTex[DRAW_TEX]);
    swap(_velocityAtt[FETCH_TEX], _velocityAtt[DRAW_TEX]);
    swap(_pressureTex[FETCH_TEX], _pressureTex[DRAW_TEX]);
    swap(_pressureAtt[FETCH_TEX], _pressureAtt[DRAW_TEX]);

    _frontierShader.popProgram();
}

void FluidCharacter::drawFluid()
{
    _drawShader.pushProgram();

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _heatTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _pressureTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _velocityTex[FETCH_TEX]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dyeTex[FETCH_TEX]);

    glPointSize(POINT_SIZE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPointSize(1.0f);

    _drawShader.popProgram();
}

void FluidCharacter::exitStage()
{
    play().view()->camera3D()->unregisterObserver(*this);

    play().propTeam2D()->deleteImageHud(_statsPanel);
    play().propTeam2D()->deleteTextHud(_fps);
    play().propTeam2D()->deleteTextHud(_ups);
}

bool FluidCharacter::keyPressEvent(const KeyboardEvent &event)
{
    if(event.getAscii() == 'R')
    {
        play().restart();
        return true;
    }
    else if(event.getAscii() == 'S')
    {
        _fps->setIsVisible(!_statsPanel->isVisible());
        _ups->setIsVisible(!_statsPanel->isVisible());
        _statsPanel->setIsVisible(!_statsPanel->isVisible());
    }

    return false;
}

bool FluidCharacter::mousePressEvent(const scaena::MouseEvent &event)
{
    if(event.button() == EMouseButton::LEFT)
    {
        moveCandleTo(event.position());
        return true;
    }

    return false;
}

bool FluidCharacter::mouseMoveEvent(const scaena::MouseEvent &event)
{
    if(play().synchronousMouse()->buttonIsPressed(EMouseButton::LEFT))
    {
        moveCandleTo(event.position());
        return true;
    }

    return false;
}

void FluidCharacter::moveCandleTo(const glm::ivec2& position)
{
    glm::ivec2 viewport = play().view()->viewport();
    glm::vec2 candlePos(position.x, viewport.y - position.y);
    candlePos *= 2.0f / POINT_SIZE;
    _heatShader.pushProgram();
    _heatShader.setVec2f("MousePos", candlePos);
    _heatShader.popProgram();
    cout << "(" << candlePos.x << ", " << candlePos.y << ")" << endl;
}

void FluidCharacter::notify(cellar::CameraMsg &)
{
}
