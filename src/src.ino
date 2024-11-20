#include "ex.h"

Graphics gfx;
Terminal trm;
UserTerminal ut;


void appSystemFunc(){ gfx.print("test1", 30, 30); }
void appUserFunc()  { gfx.print("test2", 30, 40); }


void setup()
{  
    gfx.initializationSystem();
}

void loop()
{  
    ut.addTask({1,"sys",appSystemFunc});
    ut.addTask({2,"usr",appUserFunc});
    ut.runTask();
    
    trm.terminal(); 
}