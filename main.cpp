#include <set>
#include <string>
#include <iostream>

#include <QApplication>

#include <CellarWorkbench/Misc/Log.h>

#include <ScaenaApplication/Application.h>
#include <ScaenaApplication/GlMainWindow.h>
#include <Scaena/Play/TrivialPlay.h>

#include "DemoChooserDialog/DemoChooserDialog.h"
#include "CpuRaytracing/CpuRaytracingPlay.h"
#include "Physics2D/Physics2DPlay.h"
#include "VolumeRendering/VolumeRenderingPlay.h"

using namespace std;
using namespace cellar;
using namespace scaena;

int main(int argc, char* argv[])
{
    getLog().setOuput(cout);

    // Install available demos
    set<shared_ptr<AbstractPlay>> demos;
    demos.insert(shared_ptr<AbstractPlay>(new CpuRaytracingPlay()));
    demos.insert(shared_ptr<AbstractPlay>(new Physics2DPlay()));
    demos.insert(shared_ptr<AbstractPlay>(new VolumeRenderingPlay()));

    // Init application
    Application& app = getApplication();
    app.init(argc, argv);

    // Launch demo chooser dialog
    DemoChooserDialog dialog(demos);
    dialog.exec();

    if(dialog.userQuit())
    {
        // Something bad happened
        // or user did not want to launch a demo
        return 0;
    }

    // Retreive and setup selected demo
    app.setPlay(dialog.selectedDemo());

    // Instantiate OpenGL stage
    QGLStage* stage = new QGLStage();
    app.addCustomStage(stage);
    app.chooseStage(stage->id());

    // Create and setup default
    GlMainWindow window(stage);
    window.setGlWindowSpace(800, 600);
    window.centerOnScreen();
    window.show();

    stage->setDrawSynch( false );
    stage->setUpdateInterval( 0 );
    stage->setDrawInterval( 0 );

    return app.execute();
}
