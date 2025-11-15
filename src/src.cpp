#include "ex.h"
#include "user_xbm.h"
#include "rxtx.h"

void form_test()
{
    exForm *form_test = new exForm();

    eTextBox *textbox1 = new eTextBox("I've missed more than 9000 shots in my career. I've lost almost 300 games. 26 times, I've been trusted to take the game winning shot and missed. I've failed over and over and over again in my life. And that is why I succeed.",
                                      BorderStyle::noBorder, 210, 100, 0, 0);

    form_test->title = "Form Test. Output text";
    form_test->eFormShowMode = NORMAL;
    form_test->addElement(textbox1);

    formsStack.push(form_test);
}

void setup()
{  
    initializationSystem();
    _TD.addTask({"airplane", &form_test, _ICON.processor, DESKTOP, 0, false});
}

void loop()
{  
    _TD.terminal();
}