#include "ex.h"
#include "user_xbm.h" 
#include "rxtx.h" // Подключаем библиотеку для работы с UART

Graphics gfx; Terminal trm;
TaskDispatcher dispatcher; UserIcon icons;

struct Data
{
    short a, b;
    bool s;
};

Data dt = {10, 20, false};

void userTest()
{
    exForm *userForm = new exForm();

    eButton *button1 = new eButton("Test DataPort", [](){

    DATATX<Data> *transmitter;
    transmitter = new DATATX<Data>(&dt);
    transmitter->send();
    
    }, 5, 5);

    userForm->title = "User form";
    userForm->eFormShowMode = FULLSCREEN;

    userForm->addElement(button1);

    formsStack.push(userForm);
}

void setup()
{  
    gfx.initializationSystem();
    dispatcher.addTask({"User test", userTest, icons.MyComPort, DESKTOP, 0, false});
}

void loop()
{  
    dispatcher.terminal3();
}