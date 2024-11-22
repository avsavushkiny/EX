#include "ex.h"

Graphics gfx;
Terminal trm;
Application app1;

#define USER

void appSystemFunc(){ gfx.print("test1", 30, 30); }
void appUserFunc()
{
    app1.window("My User Application", 400, appSystemFunc);
}


void setup()
{  
    gfx.initializationSystem();
    addUserTask({1,"sys",appUserFunc});
}

void loop()
{     
    trm.terminal(); 
}