#include "Visualizer.h"

#include <cmath>

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Misc/CellarUtils.h>
#include <CellarWorkbench/Algorithm/Noise.h>
using namespace cellar;

#include <MediaWorkbench/Image/Image.h>
#include <MediaWorkbench/Image/ImageBank.h>
using namespace media;

#include <PropRoom2D/Hud/TextHud.h>
#include <PropRoom2D/PropTeam/AbstractPropTeam.h>

#include <Scaena/Stage/AbstractStage.h>
#include <Scaena/Stage/Event/StageTime.h>
#include <Scaena/Stage/Event/MouseEvent.h>
#include <Scaena/Stage/Event/SynchronousMouse.h>
using namespace scaena;


Visualizer::Visualizer(scaena::AbstractStage& stage) :
    AbstractCharacter(stage, "Visualizer"),
    _fps(stage.propTeam2D().createTextHud()),
    _skyBoxRenderer(),
    _dataRenderer(),
    _dataBox(),
    _skyBox(),
    _backgroundColor(0.0, 0.0, 0.0),
    _dataSize(128, 128, 128),
    _projection(),
    _view(),
    _eye(0.0, 0.5, 3.0),
    _shell(),
    _boil(),
    _sinNoise(),
    _ballFloor(),
    _volume(_boil),
    _light(glm::vec3(PI/4.0f, 0.5f, 3.0f), // Light Position
           glm::vec3(1.0, 1.0, 0.0),       // Light Color
           100.0f,                     // Shininess
           0.1f,                       // Ambient Contribution
           false),                     // Compute shadows
    _moveLight(false),
    _moveCamera(false)
{
    _fps->setHeight(20);
}

Visualizer::~Visualizer()
{

}

void Visualizer::enterStage()
{
    glClearColor(_backgroundColor.x,
                 _backgroundColor.y,
                 _backgroundColor.z,
                 0.0);

    initVolumes();
    initCubeMap();

    GlVbo3Df dataBoxVertices = getBoxVertices(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
    dataBoxVertices.target =
    _dataBox.createBuffer("position", dataBoxVertices);

    GlVbo3Df envBoxVertices = getBoxVertices(glm::vec3(-1.0f, -1.0f, -1.0f)*0.2f,
                                             glm::vec3( 1.0f,  1.0f,  1.0f)*0.2f);
    _skyBox.createBuffer("position", envBoxVertices);


    GlInputsOutputs dataRendererInOut;
    dataRendererInOut.setInput(dataBoxVertices.attribLocation, "position");
    dataRendererInOut.setOutput(0, "Fragment");
    _dataRenderer.setInAndOutLocations(dataRendererInOut);
    _dataRenderer.addShader(GL_VERTEX_SHADER,   ":/VolumeRendering/shaders/render.vert");
    _dataRenderer.addShader(GL_FRAGMENT_SHADER, ":/VolumeRendering/shaders/render.frag");
    _dataRenderer.link();
    _dataRenderer.pushProgram();
    _dataRenderer.setInt("OpticalSampler",   0);
    _dataRenderer.setInt("MaterialSampler",  1);
    _dataRenderer.setInt("EnvironmentSampler", 2);
    _dataRenderer.setVec3f("BackgroundColor", _backgroundColor);
    _dataRenderer.setVec3f("LightColor",   _light.color);
    _dataRenderer.setFloat("LightShine",   _light.shininess);
    _dataRenderer.setFloat("LightAmbient", _light.ambientContribution);
    _dataRenderer.setInt("ComputeShadow",  _light.isCastingShadows);
    _dataRenderer.setFloat("ds", 1.0f / _dataSize.x);
    _dataRenderer.popProgram();

    GlInputsOutputs envRendererInOut;
    envRendererInOut.setInput(envBoxVertices.attribLocation, "position");
    envRendererInOut.setOutput(0, "Fragment");
    _skyBoxRenderer.setInAndOutLocations(envRendererInOut);
    _skyBoxRenderer.addShader(GL_VERTEX_SHADER,   ":/VolumeRendering/shaders/env.vert");
    _skyBoxRenderer.addShader(GL_FRAGMENT_SHADER, ":/VolumeRendering/shaders/env.frag");
    _skyBoxRenderer.link();
    _skyBoxRenderer.pushProgram();
    _skyBoxRenderer.setInt("EnvironmentSampler", 2);
    _skyBoxRenderer.popProgram();



    _projection = glm::perspectiveFov(
        1.0f,
        (float) stage().width(),
        (float) stage().height(),
        0.1f, 10.0f);

    updateMatrices();
    updateLightPos();
}

media::GlVbo3Df Visualizer::getBoxVertices(const glm::vec3& from, const glm::vec3& to)
{
    GlVbo3Df boxVertices;
    boxVertices.attribLocation = 0;

    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   from.z));

    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   from.z));

    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     from.z));

    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   from.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     from.z));

    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   from.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     from.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   from.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   from.z));

    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   from.y,   to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(to.x,   to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, to.y,     to.z));
    boxVertices.dataArray.push_back(glm::vec3(from.x, from.y,   to.z));

    return boxVertices;
}

void Visualizer::initVolumes()
{
    int nbVoxels = _dataSize.x * _dataSize.y * _dataSize.z;
    float ds = 1.0f / _dataSize.x;

    std::vector<glm::vec4> optValues(nbVoxels);
    std::vector<glm::vec4> matValues(nbVoxels);
    int idx = 0;
    for(int k=0; k<_dataSize.z; ++k)
    {
        for(int j=0; j<_dataSize.y; ++j)
        {
            for(int i=0; i<_dataSize.x; ++i)
            {
                float x = i / (float) _dataSize.x;
                float y = j / (float) _dataSize.y;
                float z = k / (float) _dataSize.z;

                optValues[idx] = _volume.opticalAt(x, y, z, ds);
                matValues[idx] = _volume.materialAt(x, y, z, ds);
                ++idx;
            }
        }
    }


    glGenTextures(1, &_optTex);
    glBindTexture(GL_TEXTURE_3D, _optTex);
    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        GL_RGBA32F,
        _dataSize.x,
        _dataSize.y,
        _dataSize.z,
        0,
        GL_RGBA,
        GL_FLOAT,
        optValues.data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    glGenTextures(1, &_matTex);
    glBindTexture(GL_TEXTURE_3D, _matTex);
    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        GL_RGBA32F,
        _dataSize.x,
        _dataSize.y,
        _dataSize.z,
        0,
        GL_RGBA,
        GL_FLOAT,
        matValues.data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Visualizer::initCubeMap()
{
    const int NB_IMAGES = 6;
    std::string imageNames[NB_IMAGES] = {
        ":/VolumeRendering/textures/sea_x+.png",
        ":/VolumeRendering/textures/sea_x-.png",
        ":/VolumeRendering/textures/sea_y+.png",
        ":/VolumeRendering/textures/sea_y-.png",
        ":/VolumeRendering/textures/sea_z+.png",
        ":/VolumeRendering/textures/sea_z-.png"
    };

    GLenum imagePositions[NB_IMAGES] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    glGenTextures(1, &_skyBoxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyBoxTex);

    for(int i=0; i<NB_IMAGES; ++i)
    {
        Image& img = getImageBank().getImage(imageNames[i]);

        glTexImage2D(
            imagePositions[i],
            0,
            4,
            img.width(),
            img.height(),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            img.pixels());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Visualizer::beginStep(const StageTime &time)
{
}

void Visualizer::draw(const StageTime &time)
{
    _fps->setText("FPS: " + toString(floor(time.framesPerSecond())));


    glDisable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyBoxTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _matTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _optTex);

    glEnable(GL_CULL_FACE);


    _skyBoxRenderer.pushProgram();
    _skyBox.bind();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glFrontFace(GL_CW);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glFrontFace(GL_CCW);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    _skyBox.unbind();
    _skyBoxRenderer.popProgram();

    _dataRenderer.pushProgram();
    _dataBox.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    _dataBox.unbind();
    _dataRenderer.popProgram();
}

void Visualizer::exitStage()
{

}

bool Visualizer::mousePressEvent(const MouseEvent& event)
{
    switch(event.button())
    {
    case EMouseButton::LEFT  : _moveLight = true;  break;
    case EMouseButton::RIGHT : _moveCamera = true; break;
    default: break;
    }

    return true;
}

bool Visualizer::mouseReleaseEvent(const MouseEvent& event)
{
    switch(event.button())
    {
    case EMouseButton::LEFT  : _moveLight = false;  break;
    case EMouseButton::RIGHT : _moveCamera = false; break;
    default: break;
    }

    return true;
}

bool Visualizer::mouseMoveEvent(const MouseEvent&)
{
    glm::ivec2 displacement(stage().synchronousMouse().xDisplacement(),
                            stage().synchronousMouse().yDisplacement());

    const double speed = 1 / 75.0;

    if(_moveLight)
    {
        _light.position.x = modulate(_light.position.x - displacement.x * speed, -PI, PI);
        _light.position.y = clamp(_light.position.y + displacement.y * speed, -1.5, 1.5);
        updateLightPos();
    }
    else if(_moveCamera)
    {
        _eye.x = modulate(_eye.x - displacement.x * speed, -PI, PI);
        _eye.y = clamp(_eye.y + displacement.y * speed, -1.5, 1.5);
        updateMatrices();
    }

    draw(StageTime(0.0, 0.0, 1.0, true));
    return true;
}

void Visualizer::updateMatrices()
{
    glm::vec4 from4 = glm::rotate(glm::mat4(), _eye.x, glm::vec3(0.0f, 0.0f, 1.0f)) *
                      glm::rotate(glm::mat4(), _eye.y, glm::vec3(1.0f, 0.0f, 0.0f)) *
                      glm::vec4(0.0f, _eye.z, 0.0f, 1.0f);
    glm::vec3 from = glm::vec3(from4);

    _view = glm::lookAt(from, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    glm::mat4 projectionView = _projection * _view;

    _dataRenderer.pushProgram();
    _dataRenderer.setMat4f("ProjectionViewMatrix", projectionView);
    _dataRenderer.setVec3f("EyePos", from);
    _dataRenderer.popProgram();

    _skyBoxRenderer.pushProgram();
    _skyBoxRenderer.setMat4f("ProjectionMatrix", _projection);
    _skyBoxRenderer.setMat4f("ViewMatrix", _view);
    _skyBoxRenderer.popProgram();
}

void Visualizer::updateLightPos()
{
    glm::vec4 pos = glm::rotate(glm::mat4(), _light.position.x, glm::vec3(0.0f, 0.0f, 1.0f)) *
                    glm::rotate(glm::mat4(), _light.position.y, glm::vec3(1.0f, 0.0f, 0.0f)) *
                    glm::vec4(0.0f, _light.position.z, 0.0f, 0.0f);

    _dataRenderer.pushProgram();
    _dataRenderer.setVec3f("LightPos", glm::vec3(pos));
    _dataRenderer.popProgram();
}
