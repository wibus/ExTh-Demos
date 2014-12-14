#include "Visualizer.h"

#include <cmath>

#include <Misc/CellarUtils.h>
#include <Algorithm/Noise.h>

#include <Image/Image.h>
#include <Image/ImageBank.h>

#include <Hud/TextHud.h>
#include <PropTeam/AbstractPropTeam.h>

#include <Stage/AbstractStage.h>
#include <Stage/Event/StageTime.h>
#include <Stage/Event/MouseEvent.h>
#include <Stage/Event/SynchronousMouse.h>

using namespace cellar;
using namespace media;
using namespace scaena;


Visualizer::Visualizer(scaena::AbstractStage& stage) :
    AbstractCharacter(stage, "Visualizer"),
    _fps(stage.propTeam2D().createTextHud()),
    _envRenderer(),
    _dataRenderer(),
    _dataBox(),
    _envBox(),
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
    _light(Vec3f(PI/4.0f, 0.5f, 3.0f), // Light Position
           Vec3f(1.0, 1.0, 0.0),       // Light Color
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
    glClearColor(_backgroundColor.x(),
                 _backgroundColor.y(),
                 _backgroundColor.z(),
                 0.0);

    initVolumes();
    initCubeMap();

    GlVbo3Df dataBoxVertices = getBoxVertices(Vec3f(0, 0, 0), Vec3f(1, 1, 1));
    _dataBox.createBuffer("position", dataBoxVertices);

    GlVbo3Df envBoxVertices = getBoxVertices(Vec3f(-1.0f, -1.0f, -1.0f)*0.2f,
                                             Vec3f( 1.0f,  1.0f,  1.0f)*0.2f);
    _envBox.createBuffer("position", envBoxVertices);


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
    _dataRenderer.setFloat("ds", 1.0f / _dataSize.x());
    _dataRenderer.popProgram();

    GlInputsOutputs envRendererInOut;
    envRendererInOut.setInput(envBoxVertices.attribLocation, "position");
    envRendererInOut.setOutput(0, "Fragment");
    _envRenderer.setInAndOutLocations(envRendererInOut);
    _envRenderer.addShader(GL_VERTEX_SHADER,   ":/VolumeRendering/shaders/env.vert");
    _envRenderer.addShader(GL_FRAGMENT_SHADER, ":/VolumeRendering/shaders/env.frag");
    _envRenderer.link();
    _envRenderer.pushProgram();
    _envRenderer.setInt("EnvironmentSampler", 2);
    _envRenderer.popProgram();


    _projection = perspective(1.0f, stage().width()/(float)stage().height(), 0.1f, 10.0f);
    updateMatrices();
    updateLightPos();
}

media::GlVbo3Df Visualizer::getBoxVertices(const Vec3f& from, const Vec3f& to)
{
    GlVbo3Df boxVertices;
    boxVertices.attribLocation = 0;
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   from.z()));

    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   from.z()));

    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     from.z()));

    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   from.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     from.z()));

    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   from.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     from.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   from.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   from.z()));

    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   from.y(),   to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(to.x(),   to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), to.y(),     to.z()));
    boxVertices.dataArray.push_back(Vec3f(from.x(), from.y(),   to.z()));
    return boxVertices;
}

void Visualizer::initVolumes()
{
    int nbVoxels = _dataSize.x() * _dataSize.y() * _dataSize.z();
    float ds = 1.0f / _dataSize.x();

    std::vector<cellar::Vec4f> optValues(nbVoxels);
    std::vector<cellar::Vec4f> matValues(nbVoxels);
    int idx = 0;
    for(int k=0; k<_dataSize.z(); ++k)
    {
        for(int j=0; j<_dataSize.y(); ++j)
        {
            for(int i=0; i<_dataSize.x(); ++i)
            {
                float x = i / (float) _dataSize.x();
                float y = j / (float) _dataSize.y();
                float z = k / (float) _dataSize.z();

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
        _dataSize.x(),
        _dataSize.y(),
        _dataSize.z(),
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
        _dataSize.x(),
        _dataSize.y(),
        _dataSize.z(),
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

    glGenTextures(1, &_envTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _envTex);

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
    glBindTexture(GL_TEXTURE_CUBE_MAP, _envTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, _matTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _optTex);

    glEnable(GL_CULL_FACE);


    _envRenderer.pushProgram();
    _envBox.bind();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glFrontFace(GL_CW);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    _envBox.unbind();
    _envRenderer.popProgram();

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
    Vec3i displacement(stage().synchronousMouse().xDisplacement(),
                       stage().synchronousMouse().yDisplacement());

    const double speed = 1 / 75.0;

    if(_moveLight)
    {
        _light.position.setX( modulate(_light.position.x() - displacement.x() * speed, -PI, PI) );
        _light.position.setY( clamp(_light.position.y() + displacement.y() * speed, -1.5, 1.5) );
        updateLightPos();
    }
    else if(_moveCamera)
    {
        _eye.setX( modulate(_eye.x() - displacement.x() * speed, -PI, PI) );
        _eye.setY( clamp(_eye.y() + displacement.y() * speed, -1.5, 1.5) );
        updateMatrices();
    }

    draw(StageTime(0.0, 0.0, 1.0, true));
    return true;
}

void Visualizer::updateMatrices()
{
    Vec3f from = rotate(0.0f, 0.0f, 1.0f, _eye.x()) *
                 rotate(1.0f, 0.0f, 0.0f, _eye.y()) *
                 Vec4f(0.0f, _eye.z(), 0.0f, 0.0f);
    _view = lookAt(from, Vec3f(0, 0, 0), Vec3f(0, 0, 1));
    Mat4f projectionView = _projection * _view;

    _dataRenderer.pushProgram();
    _dataRenderer.setMat4f("ProjectionViewMatrix", projectionView);
    _dataRenderer.setVec3f("EyePos", from);
    _dataRenderer.popProgram();

    _envRenderer.pushProgram();
    _envRenderer.setMat4f("ProjectionMatrix", _projection);
    _envRenderer.setMat4f("ViewMatrix", _view);
    _envRenderer.popProgram();
}

void Visualizer::updateLightPos()
{
    Vec3f pos = rotate(0.0f, 0.0f, 1.0f, _light.position.x()) *
                rotate(1.0f, 0.0f, 0.0f, _light.position.y()) *
                Vec4f(0.0f, _light.position.z(), 0.0f, 0.0f);
    _dataRenderer.pushProgram();
    _dataRenderer.setVec3f("LightPos", pos);
    _dataRenderer.popProgram();
}
