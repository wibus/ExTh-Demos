#include <iostream>

#include <Misc/Log.h>

#include <ScaenaApplication/Application.h>
#include <ScaenaApplication/GlMainWindow.h>
#include <Stage/QGLStage.h>
#include <Play/TrivialPlay.h>

#include "FractalsCharacter.h"

using namespace std;
using namespace cellar;
using namespace scaena;

#include <Misc/CellarUtils.h>
#include <Image/Image.h>
#include <Algorithm/Noise.h>
#include <math.h>
#include <time.h>



int main(int argc, char** argv) try
{
    getLog().setOuput(cout);
    getApplication().init(argc, argv);

    Q_INIT_RESOURCE(fractals);

    QGLStage* stage = new QGLStage();
    stage->setDrawSynch(false);
    stage->setDrawInterval(10000);
    stage->setUpdateInterval(10);
    getApplication().addCustomStage(stage);

    GlMainWindow window(stage);
    window.setGlWindowSpace(900, 900);
    window.centerOnScreen();
    window.show();

    shared_ptr<Character> character(new FractalsCharacter(*stage));
    shared_ptr<AbstractPlay> play(new TrivialPlay("Fluid2D",character));
    getApplication().setPlay(play);

    return getApplication().execute(stage->id());
}
catch(exception& e)
{
    cerr << "Exception caught : " << e.what() << endl;
}
catch(...)
{
    cerr << "Exception passed threw.." << endl;
    throw;
}
