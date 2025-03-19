#include "ex.h"
#include "ex_xbm.h" 
#include "rxtx.h" // Подключаем библиотеку для работы с UART

Graphics gfx;
Terminal trm;
TaskDispatcher dispatcher; Icon icons;

struct Data
{
    short a, b; char t[20]; bool s; bool d;
};

Data dt = {10, 20, "hello", true, true};
Data rxd;

void userTest()
{
    exForm *userForm = new exForm();

    eButton *button1 = new eButton("Test DataPort", [](){

    DATATX transmitter;
    transmitter.sendData(dt, 0);

    InstantMessage message("Data sent!", 2000); message.show();
    }, 5, 5);

    eFunction *func1 = new eFunction([]()
                                     {
    DATARX receiver;
    if (receiver.receive(rxd, 0))
    {
        InstantMessage message("Data receive!", 2000);
        message.show();
    }
    });

    userForm->title = "User form";
    userForm->eFormShowMode = FULLSCREEN;

    userForm->addElement(button1);
    userForm->addElement(func1);

    formsStack.push(userForm);
}

void setup()
{  
    gfx.initializationSystem();
    dispatcher.addTask({"User test", userTest, icons.briefcase, DESKTOP, 0, false});
}

void loop()
{  
    dispatcher.terminal3();
}