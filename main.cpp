#include <set>
#include <string>

#include <QApplication>

#include <ScaenaApplication/Application.h>
#include <ScaenaApplication/GlMainWindow.h>

#include "DemoChooserDialog/DemoChooserDialog.h"
#include "Physics2D/Physics2DPlay.h"

using namespace std;
using namespace scaena;

int main(int argc, char* argv[])
{
    // Install available demos
    set<shared_ptr<AbstractPlay>> demos;
    demos.insert(shared_ptr<AbstractPlay>(new Physics2DPlay()));

    // Fake plays
    demos.insert(shared_ptr<AbstractPlay>(new Physics2DPlay()));
    demos.insert(shared_ptr<AbstractPlay>(new Physics2DPlay()));

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
    stage->setUpdateInterval( 1 );
    stage->setDrawInterval( 16 );

    return app.execute();
}
