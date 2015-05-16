#ifndef VOLUME_RENDERING_VISUALIZER_H
#define VOLUME_RENDERING_VISUALIZER_H

#include <CellarWorkbench/GL/GlProgram.h>
#include <CellarWorkbench/GL/GlVao.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>

#include <Scaena/Play/Character.h>

#include "Volumes.h"
#include "Lights.h"


class Visualizer :
        public scaena::Character
{
public:
    Visualizer();
    virtual ~Visualizer();

    virtual void enterStage() override;
    virtual void beginStep(const scaena::StageTime &time) override;
    virtual void draw(const std::shared_ptr<scaena::View> &view,
                      const scaena::StageTime&time) override;
    virtual void exitStage() override;

    virtual bool mousePressEvent(const scaena::MouseEvent &event) override;
    virtual bool mouseReleaseEvent(const scaena::MouseEvent &event) override;
    virtual bool mouseMoveEvent(const scaena::MouseEvent &event) override;

    virtual void updateMatrices();
    virtual void updateLightPos();

protected:
    virtual cellar::GlVbo3Df getBoxVertices(const glm::vec3& from,
                                           const glm::vec3& to);
    virtual void initVolumes();
    virtual void initCubeMap();

private:
    std::shared_ptr<prop2::TextHud> _fps;
    cellar::GlProgram _skyBoxRenderer;
    cellar::GlProgram _dataRenderer;
    cellar::GlVao _dataBox;
    cellar::GlVao _skyBox;

    glm::vec3 _backgroundColor;
    glm::ivec3 _dataSize;
    unsigned int _optTex;
    unsigned int _matTex;
    unsigned int _skyBoxTex;

    glm::mat4 _projection;
    glm::mat4 _view;
    glm::vec3 _eye;
    glm::vec3 _lgt;

    Shell _shell;
    Boil  _boil;
    SinNoise _sinNoise;
    BallFloor _ballFloor;
    IVolume& _volume;
    Light _light;

    bool _moveLight;
    bool _moveCamera;
};

#endif //VOLUME_RENDERING_VISUALIZER_H
