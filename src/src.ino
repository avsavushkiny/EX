#include "ex.h"
#include "user_xbm.h"

Graphics gfx;
Terminal trm;

TaskDispatcher dispatcher; UserIcon icons;

void f()
{
    exForm *userForm = new exForm();

    userForm->title = "User form";
    userForm->eFormShowMode = FULLSCREEN;

    formsStack.push(userForm);
}

void setup()
{  
    gfx.initializationSystem();
    dispatcher.addTask({"f", f, icons.MyMessage, DESKTOP, 0, false});
}

void loop()
{  
    dispatcher.terminal3();
}