#include "CpuRaytracingCharacter.h"

#include <CellarWorkbench/Misc/CellarUtils.h>
using namespace cellar;

#include <PropRoom2D/PropTeam/AbstractPropTeam.h>
#include <PropRoom2D/Hud/TextHud.h>
using namespace prop2;

#include <Stage/AbstractStage.h>
#include <Stage/Event/StageTime.h>
using namespace scaena;


CpuRaytracingCharacter::CpuRaytracingCharacter(AbstractStage &stage) :
    AbstractCharacter(stage, "CPU Raytracing Character")
{
}

void CpuRaytracingCharacter::enterStage()
{
    _fps = stage().propTeam2D().createTextHud();
    _fps->setText("FPS: ");
    _fps->setHandlePosition(Vec2r(18.0, -30.0));
    _fps->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _fps->setVerticalAnchor(EVerticalAnchor::TOP);
    _fps->setHeight(15);

    _ups = stage().propTeam2D().createTextHud();
    _ups->setText("UPS: ");
    _ups->setHandlePosition(Vec2r(18.0, -50.0));
    _ups->setHorizontalAnchor(EHorizontalAnchor::LEFT);
    _ups->setVerticalAnchor(EVerticalAnchor::TOP);
    _ups->setColor(Vec4r(0.3, 0.4, 1.0, 1.0));
    _ups->setHeight(15);
}

void CpuRaytracingCharacter::beginStep(const StageTime &time)
{
    _ups->setText("UPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void CpuRaytracingCharacter::draw(const StageTime &time)
{
    _fps->setText("FPS: " + toString(floor(1.0 / time.elapsedTime())));
}

void CpuRaytracingCharacter::exitStage()
{
    stage().propTeam2D().deleteTextHud(_fps);
    stage().propTeam2D().deleteTextHud(_ups);
}
