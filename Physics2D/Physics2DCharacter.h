#ifndef PHYSICS2D_CHARACTER_H
#define PHYSICS2D_CHARACTER_H

#include <memory>

#include <Scaena/Play/Character.h>

namespace cellar
{
    class CameraManBird;
}

namespace prop2
{
    class Circle;
    class Polygon;
    class TextHud;
    class ImageHud;
}


class Physics2DCharacter : public scaena::Character
{
public:
    Physics2DCharacter();

    virtual void enterStage() override;
    virtual void beginStep(const scaena::StageTime &time) override;
    virtual void draw(const std::shared_ptr<scaena::View> &view,
                      const scaena::StageTime&time) override;
    virtual void exitStage() override;


private:
    std::shared_ptr<cellar::CameraManBird> _camMan;

    std::shared_ptr<prop2::Polygon> _background;
    std::shared_ptr<prop2::Polygon> _groundFloor;
    std::shared_ptr<prop2::Polygon> _groundLeft;
    std::shared_ptr<prop2::Polygon> _groundRight;
    std::shared_ptr<prop2::Polygon> _ramp;

    std::shared_ptr<prop2::Polygon> _housePillarLeft;
    std::shared_ptr<prop2::Polygon> _housePillarRight;
    std::shared_ptr<prop2::Polygon> _houseRoof;

    std::shared_ptr<prop2::Polygon> _gear;
    std::shared_ptr<prop2::Circle> _ball;

    std::shared_ptr<prop2::ImageHud> _statsPanel;
    std::shared_ptr<prop2::TextHud> _fps;
    std::shared_ptr<prop2::TextHud> _ups;    
};

#endif // PHYSICS2D_CHARACTER_H
