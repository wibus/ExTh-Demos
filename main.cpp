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
#include <Scaena/ScaenaApplication/GlMainWindow.h>
#include <Scaena/ScaenaApplication/QGlWidgetView.h>

#include "DemoChooserDialog/DemoChooserDialog.h"
#include "CpuRaytracing/CpuRaytracingCharacter.h"
#include "CpuRaytracing/QPostProdManager.h"
#include "Physics2D/Physics2DCharacter.h"
#include "VolumeRendering/Visualizer.h"
#include "Fractal/FractalCharacter.h"
#include "Fluid2D/FluidCharacter.h"

using namespace std;
using namespace cellar;
using namespace prop3;
using namespace scaena;

std::shared_ptr<QMainWindow> mainWindow;

std::shared_ptr<Play> buildCpuRaytracing()
{
    std::shared_ptr<Play> play(new Play("CPU Raytracing"));
    std::shared_ptr<Character> character(new CpuRaytracingCharacter(*play));
    std::shared_ptr<Act> act(new Act(*play, "Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Create and setup default
    QPostProdManager* postProdManager = new QPostProdManager();
    postProdManager->setFixedSize(postProdManager->size());
    std::shared_ptr<PostProdManager> postProdPtr(postProdManager);

    QGlWidgetView* view = new QGlWidgetView("Main View");
    std::shared_ptr<View> viewPtr(view);
    view->setMinimumSize(64, 64);
    view->setFixedSize(850, 500);

    view->setPostProdManager(postProdPtr);
    play->addView(viewPtr);

    QVBoxLayout* viewlayout = new QVBoxLayout();
    viewlayout->addStretch();
    viewlayout->addWidget(view);
    viewlayout->addStretch();

    QWidget* centralWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    centralWidget->setLayout(layout);
    layout->addWidget(postProdManager);
    layout->addStretch();
    layout->addLayout(viewlayout);
    layout->addStretch();

    GlMainWindow* window = new GlMainWindow(play, centralWidget);
    window->centerOnScreen();
    window->show();

    mainWindow.reset(window);

    return play;
}

std::shared_ptr<Play> buildVolumeRendering()
{
    std::shared_ptr<Play> play(new Play("Volume Rendering"));
    std::shared_ptr<Character> character(new Visualizer(*play));
    std::shared_ptr<Act> act(new Act(*play, "Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Create and setup default
    GlMainWindow* window = new GlMainWindow(play);
    window->setGlWindowSpace(800, 600);
    window->centerOnScreen();
    window->show();

    mainWindow.reset(window);

    return play;
}

std::shared_ptr<Play> buildFluid2D()
{
    std::shared_ptr<Play> play(new Play("Fluid 2D"));
    play->setUpdateRate(Play::DEACTIVATE_AUTOMATIC_REFRESH);
    play->setDrawRate(Play::FASTEST_REFRESH_RATE_AVAILABLE);

    std::shared_ptr<Character> character(new FluidCharacter(*play));
    std::shared_ptr<Act> act(new Act(*play, "Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Create and setup default
    GlMainWindow* window = new GlMainWindow(play);
    window->setGlWindowSpace(
        FluidCharacter::WIDTH  * FluidCharacter::POINT_SIZE,
        FluidCharacter::HEIGHT * FluidCharacter::POINT_SIZE);
    window->centerOnScreen();
    window->show();

    mainWindow.reset(window);


    return play;
}

std::shared_ptr<Play> buildFractal()
{
    std::shared_ptr<Play> play(new Play("Fractal"));
    std::shared_ptr<Character> character(new FractalsCharacter(*play));
    std::shared_ptr<Act> act(new Act(*play, "Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Create and setup default
    GlMainWindow* window = new GlMainWindow(play);
    window->setGlWindowSpace(800, 600);
    window->centerOnScreen();
    window->show();

    mainWindow.reset(window);

    return play;
}

std::shared_ptr<Play> buildPhysics2D()
{
    std::shared_ptr<Play> play(new Play("Physics 2D"));
    std::shared_ptr<Character> character(new Physics2DCharacter(*play));
    std::shared_ptr<Act> act(new Act(*play, "Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Create and setup default
    GlMainWindow* window = new GlMainWindow(play);
    window->setGlWindowSpace(800, 600);
    window->centerOnScreen();
    window->show();

    mainWindow.reset(window);

    return play;
}

int main(int argc, char* argv[])
{
    getLog().setOuput(cout);

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
    mainWindow.reset();
    return exitCode;
}
