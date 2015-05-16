#ifndef FLUID_CHARACTER_H
#define FLUID_CHARACTER_H

#include <memory>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>
#include <CellarWorkbench/DesignPattern/SpecificObserver.h>
#include <CellarWorkbench/GL/GlProgram.h>
#include <CellarWorkbench/GL/GlVao.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>
#include <PropRoom2D/Prop/Hud/ImageHud.h>

#include <Scaena/Play/Character.h>


class FluidCharacter : public scaena::Character,
                       public cellar::SpecificObserver<cellar::CameraMsg>
{
public:
    FluidCharacter();
    virtual ~FluidCharacter();

    virtual void enterStage() override;
    virtual void beginStep(const scaena::StageTime &time) override;
    virtual void endStep(const scaena::StageTime &time) override;
    virtual void draw(const std::shared_ptr<scaena::View> &view,
                      const scaena::StageTime &time) override;
    virtual void exitStage() override;

    virtual bool keyPressEvent(const scaena::KeyboardEvent &event) override;
    virtual bool mousePressEvent(const scaena::MouseEvent &event) override;
    virtual bool mouseMoveEvent(const scaena::MouseEvent &event) override;

    virtual void notify(cellar::CameraMsg &msg) override;

    static const int WIDTH;
    static const int HEIGHT;
    static const int AREA;
    static const int POINT_SIZE;


protected:
    glm::vec4 initDye(float s, float t);
    glm::vec4 initVelocity(float s, float t);
    glm::vec4 initPressure(float s, float t);
    glm::vec4 initHeat(float s, float t);
    glm::vec4 initFrontier(float s, float t);

    template<typename T>
    void initTexture(unsigned int texId, const T& img);

    void advect();
    void diffuse();
    void heat();
    void computePressure();
    void substractPressureGradient();
    void frontier();
    void drawFluid();

    void moveCandleTo(const glm::ivec2& position);


private:
    // Size
    const float DX;
    const float DT;
    const float VISCOSITY;
    const float HEATDIFF;

    // Fluid simulation GL specific attributes
    cellar::GlProgram _advectShader;
    cellar::GlProgram _heatShader;
    cellar::GlProgram _jacobiShader;
    cellar::GlProgram _divergenceShader;
    cellar::GlProgram _gradSubShader;
    cellar::GlProgram _frontierShader;
    cellar::GlProgram _drawShader;
    cellar::GlVao _vao;

    const int DRAW_TEX;
    const int FETCH_TEX;

    unsigned int _dyeTex[2];
    unsigned int _velocityTex[2];
    unsigned int _pressureTex[2];
    unsigned int _heatTex[2];
    unsigned int _frontierTex;
    unsigned int _tempDivTex;

    GLenum _dyeAtt[2];
    GLenum _velocityAtt[2];
    GLenum _pressureAtt[2];
    GLenum _heatAtt[2];

    unsigned int _fbo;

    // Stats panel (FPS, UPS)
    std::shared_ptr<prop2::ImageHud> _statsPanel;
    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;
};

#endif // FLUID_CHARACTER_H
