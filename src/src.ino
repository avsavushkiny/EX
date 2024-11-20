#include "ex.h"

Graphics gfx;
Terminal trm;


void appSystemFunc(){ gfx.print("test1", 30, 30); }
void appUserFunc()  { gfx.print("test2", 30, 40); }


void setup()
{  
    gfx.initializationSystem();
    addTask({1,"sys",appSystemFunc});
}

void loop()
{     
    trm.terminal(); 
}