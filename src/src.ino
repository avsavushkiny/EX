#include "ex.h"

Graphics gfx;
Terminal trm;

#define USER

void text()
{
    gfx.print("test", 30, 30);
}

void setup()
{  
    gfx.initializationSystem();
}

void loop()
{  
    userTask(text);
    trm.terminal(); 
}