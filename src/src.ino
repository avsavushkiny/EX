#include "ex.h"
#include "user_xbm.h" 
#include "rxtx.h" // Подключаем библиотеку для работы с UART

Graphics gfx; Terminal trm;
TaskDispatcher dispatcher; UserIcon icons;

struct Data
{
    short a, b; String t;
    bool s; bool d;
};

Data dt = {10, 20, "hello", true, true};

void userTest()
{
    exForm *userForm = new exForm();

    eButton *button1 = new eButton("Test DataPort", [](){

    DATATX<Data> *transmitter;
    transmitter = new DATATX<Data>(&dt);
    transmitter->sendData();
    InstantMessage message("Data sent!", 2000); message.show();
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