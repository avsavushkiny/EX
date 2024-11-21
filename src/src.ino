#include "ex.h"

Graphics gfx;
Terminal trm;
Application app1;


void appSystemFunc(){ gfx.print("test1", 30, 30); }
void appUserFunc()
{
    app1.window("User Application", 400, appSystemFunc);
}


void setup()
{  
    gfx.initializationSystem();
    addTask({1,"sys",appUserFunc});
}

void loop()
{     
    trm.terminal(); 
}