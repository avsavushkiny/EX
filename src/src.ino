#include "ex.h"
#include "user_xbm.h"

Graphics gfx;
Terminal trm;

TaskDispatcher dispatcher; UserIcon icons;

void userTest()
{
    exForm *userForm = new exForm();

    userForm->title = "User form";
    userForm->eFormShowMode = FULLSCREEN;

    formsStack.push(userForm);
}

void setup()
{  
    gfx.initializationSystem();
    dispatcher.addTask({"User test", userTest, icons.MyTechInfo, DESKTOP, 0, false});
}

void loop()
{  
    dispatcher.terminal3();
}