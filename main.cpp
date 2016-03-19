#include <set>
#include <string>
#include <iostream>

#include <GLM/glm.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <CellarWorkbench/Misc/Log.h>

#include <PropRoom3D/Team/ArtDirector/AbstractArtDirector.h>

#include <Scaena/Play/Act.h>
#include <Scaena/Play/Play.h>
#include <Scaena/ScaenaApplication/Application.h>
#include <Scaena/ScaenaApplication/QGlWidgetView.h>

#include "DemoChooserDialog/DemoChooserDialog.h"
#include "CpuRaytracing/CpuRaytracingCharacter.h"
#include "CpuRaytracing/RaytracerGui.h"
#include "Physics2D/Physics2DCharacter.h"
#include "VolumeRendering/Visualizer.h"
#include "Fractal/FractalCharacter.h"
#include "Fluid2D/FluidCharacter.h"

using namespace std;
using namespace cellar;
using namespace prop3;
using namespace scaena;

std::shared_ptr<QWidget> view;

std::shared_ptr<Play> buildCpuRaytracing()
{
    // Build the Play
    std::shared_ptr<Play> play(new Play("CPU Raytracing"));
    std::shared_ptr<Character> character(new CpuRaytracingCharacter());
    std::shared_ptr<Act> act(new Act("Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Build GUI
    RaytracerGui* window = new RaytracerGui(play);
    window->show();

    return play;
}

std::shared_ptr<Play> buildVolumeRendering()
{
    // Build default view
    QGlWidgetView* view = new QGlWidgetView("MainView");
    std::shared_ptr<View> pView(view);
    view->setGlWindowSpace(800, 600);
    view->centerOnScreen();
    view->show();

    // Build the Play
    std::shared_ptr<Play> play(new Play("Volume Rendering"));
    std::shared_ptr<Character> character(new Visualizer());
    std::shared_ptr<Act> act(new Act("Main Act"));
    act->addCharacter(character);
    play->appendAct(act);
    play->addView(pView);

    return play;
}

std::shared_ptr<Play> buildFluid2D()
{
    // Build default view
    QGlWidgetView* view = new QGlWidgetView("MainView");
    std::shared_ptr<View> pView(view);
    view->setGlWindowSpace(
        FluidCharacter::WIDTH  * FluidCharacter::POINT_SIZE,
        FluidCharacter::HEIGHT * FluidCharacter::POINT_SIZE);
    view->centerOnScreen();
    view->show();

    // Build the Play
    std::shared_ptr<Play> play(new Play("Fluid 2D"));
    play->setUpdateRate(Play::DEACTIVATE_AUTOMATIC_REFRESH);
    play->setDrawRate(Play::FASTEST_REFRESH_RATE_AVAILABLE);
    std::shared_ptr<Character> character(new FluidCharacter());
    std::shared_ptr<Act> act(new Act("Main Act"));
    act->addCharacter(character);
    play->appendAct(act);
    play->addView(pView);

    return play;
}

std::shared_ptr<Play> buildFractal()
{
    // Build default view
    QGlWidgetView* view = new QGlWidgetView("MainView");
    std::shared_ptr<View> pView(view);
    view->setGlWindowSpace(800, 600);
    view->centerOnScreen();
    view->show();

    // Build the Play
    std::shared_ptr<Play> play(new Play("Fractal"));
    std::shared_ptr<Character> character(new FractalsCharacter());
    std::shared_ptr<Act> act(new Act("Main Act"));
    act->addCharacter(character);
    play->appendAct(act);
    play->addView(pView);

    return play;
}

std::shared_ptr<Play> buildPhysics2D()
{
    // Build default view
    QGlWidgetView* view = new QGlWidgetView("MainView");
    std::shared_ptr<View> pView(view);
    view->setGlWindowSpace(800, 600);
    view->centerOnScreen();
    view->show();

    // Build the Play
    std::shared_ptr<Play> play(new Play("Physics 2D"));
    std::shared_ptr<Character> character(new Physics2DCharacter());
    std::shared_ptr<Act> act(new Act("Main Act"));
    act->addCharacter(character);
    play->appendAct(act);
    play->addView(pView);

    return play;
}

int main(int argc, char* argv[])
{
    // Init application
    Application& app = getApplication();
    app.init(argc, argv);

    // Available demos
    DemoVec demos;
    demos.push_back(Demo("CPU Raytracing",   &buildCpuRaytracing));
    demos.push_back(Demo("Volume Rendering", &buildVolumeRendering));
    demos.push_back(Demo("Fluid 2D",         &buildFluid2D));
    demos.push_back(Demo("Fractal",          &buildFractal));
    demos.push_back(Demo("Physics 2D",       &buildPhysics2D));

    // Launch demo chooser dialog
    DemoChooserDialog dialog(demos);
    dialog.exec();

    if(dialog.userQuit())
    {
        return 0;
    }

    // Retreive and setup selected demo
    app.setPlay(dialog.selectedDemo().second());
    int exitCode = app.execute();
    return exitCode;
}
