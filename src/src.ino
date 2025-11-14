// #include "ex.h"

// void setup()
// {
//     initializationSystem();
// }

// void loop()
// {
//     _TD.terminal();
// }

#include "ex.h"
#include "user_xbm.h"
#include "rxtx.h"

// Graphics gfx;
// TaskDispatcher dispatcher;
// Icon icons;

struct Air
{
    short time_0, time_1;
    bool state_air;
    char name_air[20];
};

Air air_tx, air_rx;
short t_0, t_1;

void airplane()
{
    exForm *form_airplane = new exForm();

    eButton *btn0 = new eButton("-", [](){}, 5, 5);
    eButton *btn1 = new eButton("+", [](){}, 41, 5);

    eButton *btn2 = new eButton("-", [](){}, 5, 23);
    eButton *btn3 = new eButton("+", [](){}, 41, 23);
    
    eButton *btn4 = new eButton("RUN", [](){}, 5, 41);

    eLabel *label0 = new eLabel("000", 21, 7);
    eLabel *label1 = new eLabel("111", 21, 25);
    //eLabel *label2 = new eLabel();
    //eLabel *label3 = new eLabel();

    eTextBox *txtBox0 = new eTextBox("Test", BorderStyle::noBorder, 80, 50, 26, 39);

    form_airplane->addElement(btn0); form_airplane->addElement(btn1); form_airplane->addElement(btn2);
    form_airplane->addElement(btn3); form_airplane->addElement(btn4);
    form_airplane->addElement(label0); form_airplane->addElement(label1);
    form_airplane->addElement(txtBox0);

    form_airplane->eFormShowMode = NORMAL;
    form_airplane->title = "Airplane v.1";
    
    formsStack.push(form_airplane);
}

struct RxTxData
{
    short a; short b;
};

RxTxData rtd;

void readDataPort()
{
    exForm *form0 = new exForm();
    form0->eFormShowMode = NORMAL;
    form0->title = "Read Data port";

    eFunction *func0 = new eFunction([](){
        DATARX r;
        if (r.receive(rtd, 0))
        {
            String text = "Done!\n" + (String)rtd.a + "\n" + (String)rtd.b;
            InstantMessage mess0(text, 2000); mess0.show();
        }
    });

    form0->addElement(func0);

    formsStack.push(form0);
}

void setup()
{  
    initializationSystem();
    // _TD.addTask({"airplane", airplane, _ICON.processor, DESKTOP, 0, false});
    // _TD.addTask({"Read dataPort", readDataPort, _ICON.processor, DESKTOP, 0, false});
}

void loop()
{  
    _TD.terminal();
}