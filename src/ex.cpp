#include "ex.h"

// Global object (объявляем как extern)
Graphics _GRF;
TaskDispatcher _TD;
GGL _GGL;
Icon _ICON;
SystemIcon _SICON;
Joystick _JOY;
Cursor _CRS;

// class Systems;
// extern Systems _SYS;

/* Global function */
/* Initialization systems */
void initializationSystem()
{
    /* 
       GPIO release from sleep

       esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 1);
       esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 1); 
       esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); // Stick 0
       esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1); // EX button
    */
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1);    // Stick 0
    
    /* setting the operating system state */
    Serial.begin(9600);
    _GGL.gray.begin();

    /* setting the resolution of the analog-to-digital converter */
    analogReadResolution(RESOLUTION_ADC);

    /* determine the backlight-port mode */
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);;
    
    /* determine the operating modes of digital ports */
    pinMode(PIN_BUTTON_ENTER, INPUT);
    pinMode(PIN_BUTTON_EX,    INPUT);
    pinMode(PIN_BUTTON_A,     INPUT);
    pinMode(PIN_BUTTON_B,     INPUT);
    pinMode(PIN_BATTERY,      INPUT);

    /* Contrast & run system element */
    // systems.setDisplayContrast(240);
    // systems.executeAllSystemElements();
    

    /*
       Vector
       moving system-task to the vector
       determine the number of tasks in the vector
    */
    // addTasksForSystems();

    // Clear buffer LCD display
    _GGL.gray.clearBuffer(); // -->

    // output text
    String text = "SozvezdiyeOs + GGL";
    _GGL.gray.writeLine(5, 123, VERSION_EX, 10, 1, _GGL.gray.BLACK);
    
    // draw gray-line
    _GGL.gray.drawHLine(0, 135, 256, _GGL.gray.LIGHT_GRAY, 5);
    _GGL.gray.drawHLine(0, 140, 256, _GGL.gray.DARK_GRAY, 5);
    _GGL.gray.drawHLine(0, 145, 256, _GGL.gray.BLACK, 5);

    // send data to display
    _GGL.gray.sendBuffer(); // <--

    delay(2500);
}
/* Null function */
void nullFunction() {}
/* Reboot ESP32 */
void _rebootBoard()
{
    ESP.restart();
}

/* Form's */

/* Form. Graphics test #1 */
void _graphicsTest1(int xG, int yG, int wG, int hG)
{   
    for (int x = 0; x < 256; x += 2)
    {
        int y = random(12, 160);
        int h = random(160 - y);
        _GGL.gray.drawVLine(x, y, h, _GGL.gray.BLACK, 1); 
    }
}
void _myGraphicsTest1()
{
    exForm *formGraphicsTest1 = new exForm;                   // [0] создали форму
    eGraphics *graphicsTest1 = new eGraphics(_graphicsTest1, 0, 0, 256, 147); // [1] создали элемент формы

    formGraphicsTest1->title = "Graphics test";    // [2] назвали форму
    formGraphicsTest1->eFormShowMode = FULLSCREEN; // [3] определили режим формы
    formGraphicsTest1->addElement(graphicsTest1);  // [4] добавили эелемент в контейнер

    formsStack.push(formGraphicsTest1); // [5] добавили элемент в стэк форм
}

/* Form. Graphics test #2 */
void _graphicsTest2(int xG, int yG, int wG, int hG)
{ 
    int w{4}, h{4};

    for (int y = yG; y < 160; y += h)
    {
        for (int x = xG; x < wG; x += w)
        {
            if ((x / w + y / h) % 2 == 0)
            {
                // u8g2.drawBox(x, y, w, h);
                _GGL.gray.drawBox(x, y, w, h, _GGL.gray.BLACK);
            }
        }
    }
}
void _myGraphicsTest2()
{
    exForm *formGraphicsTest2 = new exForm;                   // [0] создали форму
    eGraphics *graphicsTest2 = new eGraphics(_graphicsTest2, 0, 0, 256, 147); // [1] создали элемент формы

    formGraphicsTest2->title = "Graphics test 2";  // [2] назвали форму
    formGraphicsTest2->eFormShowMode = FULLSCREEN; // [3] определили режим формы
    formGraphicsTest2->addElement(graphicsTest2);  // [4] добавили эелемент в контейнер

    formsStack.push(formGraphicsTest2); // [5] добавили элемент в стэк форм
}

/* Form. Graphics test #3 */
Timer timerTest3;
const int numPoints = 12;
int points[numPoints][2];
void randomPoints()
{
    // generate random points
    for (int i = 0; i < numPoints; i++)
    {
        points[i][0] = random(10, 246);
        points[i][1] = random(20, 150);
    }
}
void _graphicsTest3(int xG, int yG, int wG, int hG)
{
    timerTest3.timer(randomPoints, 3000);
    // draw points
    for (int i = 0; i < numPoints; i++)
    {
        _GGL.gray.drawPixel(points[i][0], points[i][1], _GGL.gray.BLACK);
    }

    // draw lines
    for (int i = 0; i < numPoints; i++)
    {
        for (int j = i + 1; j < numPoints; j++)
        {
            _GGL.gray.drawLine(points[i][0], points[i][1], points[j][0], points[j][1], _GGL.gray.BLACK);
        }
    }
}
void _myGraphicsTest3()
{
    exForm *formGraphicsTest3 = new exForm;
    eGraphics *graphicsTest3 = new eGraphics(_graphicsTest3, 0, 0, 256, 147);

    formGraphicsTest3->title = "Graphics test 3";
    formGraphicsTest3->eFormShowMode = FULLSCREEN;
    formGraphicsTest3->eFormBackground = TRANSPARENT;
    formGraphicsTest3->addElement(graphicsTest3);

    formsStack.push(formGraphicsTest3);
}

/* Form. Desktop */
void _myDesktop()
{
    exForm *form0 = new exForm();

    eDesktop<TaskArguments> *desktop0 = new eDesktop<TaskArguments>(tasks);
    // ePicture *pic = new ePicture(sozos, 25, 100, 207, 38);
    // eBackground *bg0 = new eBackground(icon.pattern_1, 0, 0, 32, 32);
    
    form0->title = "Desktop";
    form0->eFormShowMode = FULLSCREEN;
    // form0->addElement(pic);
    // form0->addElement(bg0);
    form0->addElement(desktop0);

    formsStack.push(form0);
    _TD.removeTaskIndex(100);
}
/* Form. User Desktop */
// void _userDesktop()
// {
//     exForm *form1 = new exForm();
//     eDesktop<TaskArguments> *desktop1 = new eDesktop<TaskArguments>(userTasks);

//     form1->title = "User Desktop";
//     form1->eFormShowMode = FULLSCREEN;
//     form1->addElement(desktop1);

//     formsStack.push(form1);
// }

/* Form. OS information */
void _info()
{
    exForm *formInfoSystems = new exForm();

    String text1 = "Founder of the platform and\nchief developer: Aleksander SAVUSHKIN\n\n";
    String text2 = "Github: @avsavushkiny, GitVerse: @avsavushkin\n\n";
    String text3 = "Developers: Sergey KSENOFONTOV, Michail SAMOYLOV,Aleksander MICHEEV, Ekaterina SYATKINA\n\n";
    String text4 = "Phone: +7 (953) 034 4001\nE-mail: aeondc@gmail.com\n\nSozvezdiye platform\nRussia, Saransk, 2023-2025";

    eTextBox *textBoxInfo = new eTextBox(text1 + text2 + text3 + text4, BorderStyle::noBorder, 256, 150, 0, 0);
    ePicture *pic1 = new ePicture(giga, 160, 100, giga_w, giga_h);

    // text4 необходимо захватить в лямба-функцию, поэтому её помещаем в [] скобки
    
    // eFunction *func1 = new eFunction([&text4, &stateEasterEgg](){
    //     if (stateEasterEgg == true) text4 = "@catincoat, @Azcol, @fddh543";
    // });

    formInfoSystems->title = "Information";
    formInfoSystems->eFormShowMode = FULLSCREEN;
    formInfoSystems->addElement(textBoxInfo);
    formInfoSystems->addElement(pic1);
    // formInfoSystems->addElement(func1);

    formsStack.push(formInfoSystems);
}
/* Form. OS startup */
void _myOSstartupForm()
{
    exForm *formMyOSstartup = new exForm;
    eText *textMessage = new eText("I don't understand why you did this,\nbut oh well.\n\nTo launch Desktop - click on\nthe button below, good luck :))", 5, 5);
    eLine *line = new eLine(0, 97);
    eButton *button = new eButton("Run Desktop", _myDesktop, 5, 102);
    eButton *buttonReboot = new eButton("Reboot", _rebootBoard, 71, 102);
    eButton *buttonInfo = new eButton("Info", _info, 112, 102);

    // eFunction *func = new eFunction([](){
    //     if ((_joy.pressKeyEX() == true) && (_joy.pressKeyENTER() == true)) stateEasterEgg = true;
    // });

    formMyOSstartup->title = "OS startup";
    formMyOSstartup->eFormShowMode = NORMAL;

    formMyOSstartup->addElement(line);
    formMyOSstartup->addElement(textMessage);
    formMyOSstartup->addElement(button);
    formMyOSstartup->addElement(buttonReboot);
    formMyOSstartup->addElement(buttonInfo);
    // formMyOSstartup->addElement(func);

    formsStack.push(formMyOSstartup);
}
/* Form. OS hello */
void _osHello()
{
    exForm *oshello = new exForm;

    ePicture *pic1 = new ePicture(alisa_gray_5050, 10, 35, alisa_w, alisa_h);
    String text1 = "Hello, I am the operating system Sozvezdiye.\n\nI was created by students of the Children's Creativity Center 2.\n\nEnjoy!";
    eTextBox *textbox1 = new eTextBox(text1, BorderStyle::noBorder, 141, 120, 65, 10);

    oshello->title = "OS hello";
    oshello->eFormShowMode = NORMAL;
    oshello->addElement(pic1);
    oshello->addElement(textbox1);

    formsStack.push(oshello);
    _TD.removeTaskIndex(101);
}


/* Form. Test 1 */ 
void _myForm1()
{ 
    exForm *form1 = new exForm();

    eTextBox *textbox1 = new eTextBox("I've missed more than 9000 shots in my career. I've lost almost 300 games. 26 times, I've been trusted to take the game winning shot and missed. I've failed over and over and over again in my life. And that is why I succeed.", 
        BorderStyle::noBorder, 210, 100, 0, 0
    );

    form1->title = "Form 1. Output text";
    form1->eFormShowMode = NORMAL;
    form1->addElement(textbox1);


    formsStack.push(form1);
}
/* Form. Test 2 */
void _myForm2()
{
    exForm* form2 = new exForm();

    eText *text1 = new eText("My text, hello)", 5, 5);
    eButton *buttons1 = new eButton("My Button", nullFunction, 5, 20);
    eTextBox *textBox1 = new eTextBox("Test text for output in the Form", BorderStyle::shadow, 100, 30, 5, 40);
    eLinkLabel *label1 = new eLinkLabel("Label with link", nullFunction, 5, 85);
    eLine *Line2 = new eLine(0, 72);
    
    form2->addElement(text1);
    form2->addElement(buttons1);
    form2->addElement(textBox1);
    form2->addElement(label1);
    form2->addElement(Line2);

    form2->eFormShowMode = FULLSCREEN;

    formsStack.push(form2);
}
/* Form. Test 3 */
void _myForm3()
{
    exForm* form3 = new exForm();

    eText *text3 = new eText("My Form3", 5, 5);
    eButton *buttons3 = new eButton("My Button Form3", nullFunction, 5, 20);
    eTextBox *textBox3 = new eTextBox("Test text for output in the Form3", BorderStyle::shadow, 100, 30, 5, 40);
    eLinkLabel *llabel3 = new eLinkLabel("Label with link, Form3", nullFunction, 5, 75);
    eLabel *label3 = new eLabel("Settings", 5, 95);
    eLine *Line3 = new eLine(0, 100); 

    form3->title = "Form 3";
    form3->eFormShowMode = NORMAL;

    form3->addElement(text3);
    form3->addElement(buttons3);
    form3->addElement(textBox3);
    form3->addElement(llabel3);
    form3->addElement(label3);
    form3->addElement(Line3);

    formsStack.push(form3);
}

/* Form. Dispatcher [!] DELETE */
void _myDispatcherFunction(int xG, int yG, int wG, int hG)
{
    int xx{5}, yy{23};

    for (auto &_ta : tasks)
    { 
        if (_ta.activ == true)
        {
            String _Text = _ta.name;
            uint8_t sizeText = _Text.length();

            // _gfx.print(_ta.name, xx, yy, 8, 5);!!!
            
            if ((xx + (sizeText * 5) + 5) <= 240) //256 - 5 - 5
            {
                xx += (sizeText * 5) + 5;
            }
            
            if ((xx + (sizeText * 5) + 5) >= 240)
            {
                xx = 5; yy += 10;
            }
        }
    }

    xx = 5;
    
    // _gfx.print("Inactive tasks:", xx + 5, (yy + 10), 8, 5);

    yy += 20;
    
    for (auto &_ta : tasks)
    { 
        if (_ta.activ == false)
        {
            String _Text = _ta.name;
            uint8_t sizeText = _Text.length();
            
            Label labelTask;
            
            if ((xx + (sizeText * 5) + 5) <= 240) //256 - 5 - 5
            {
                xx += (sizeText * 5) + 5;
            }
            
            if ((xx + (sizeText * 5) + 5) >= 240)
            {
                xx = 5; yy += 10;
            }
        }
    }
}
void _myDispatcher()
{
    exForm *formMyDispatcher = new exForm;
    eGraphics *myDispatcher = new eGraphics(_myDispatcherFunction, 0, 0, 256, 160);

    formMyDispatcher->title = "My Dispatcher task";
    formMyDispatcher->eFormShowMode = FULLSCREEN;
    formMyDispatcher->addElement(myDispatcher);

    formsStack.push(formMyDispatcher);
}

// settings Form
// void _settingsForm()
// {
//     exForm *settingsForm = new exForm();
//     uint8_t x{0}, y{0};

//     // --> LED control
//     eCheckbox *checkLED = new eCheckbox(_SYS.STATEBACKLIGHT, "backlight control", x + 5, y + 5);

//     eFunction *funcLED = new eFunction([checkLED]()
//                                        {
//         systems.STATEBACKLIGHT = checkLED->isChecked();
//         systems.setBacklight(systems.STATEBACKLIGHT); });

//     // --> Set contrast
//     eLabel *label1 = new eLabel((String)systems.VALUECONTRAST, x + 20, y + 23);

//     eButton *button1 = new eButton("-", []()
//                                    { systems.VALUECONTRAST -= 1; delay(250); systems.setDisplayContrast(systems.VALUECONTRAST); }, x + 5, y + 20);
//     eButton *button2 = new eButton("+", []()
//                                    { systems.VALUECONTRAST += 1; delay(250); systems.setDisplayContrast(systems.VALUECONTRAST); }, x+ 50, y + 20);

//     eFunction *func1 = new eFunction([label1]()
//                                      { label1->setText((String)systems.VALUECONTRAST); });

//     eLabel *label2 = new eLabel("display contrast", x + 61, y + 23);
   
//     // --> Title, show mode Form
//     settingsForm->title = "Settings";
//     settingsForm->eFormShowMode = NORMAL;

//     // --> Add elements Form
//     settingsForm->addElement(checkLED);
//     settingsForm->addElement(funcLED);
//     settingsForm->addElement(button1);
//     settingsForm->addElement(button2);
//     settingsForm->addElement(label1);
//     settingsForm->addElement(func1);
//     settingsForm->addElement(label2);
//     // settingsForm->addElement(func2);
//     // settingsForm->addElement(label3);

//     // --> Push Form object
//     formsStack.push(settingsForm);
// }

/* Cursor */
void _systemCursor()
{
    _JOY.updatePositionXY(20);
    _CRS.cursor(true, _JOY.posX0, _JOY.posY0);

    // if ((_JOY.pressKeyEX() == true) && (_JOY.pressKeyENTER() == true))
    // {
    //     // Debug. Print coordinate
    //     // u8g2.setCursor(_joy.posX0 + 10, _joy.posY0 + 10); !!!
    //     // u8g2.print(_joy.posX0);
    //     // u8g2.setCursor(_joy.posX0 + 10, _joy.posY0 + 20);
    //     // u8g2.print(_joy.posY0);
    // }
}

/**/
TaskArguments system0[] //0 systems, 1 desktop, 2 user
{
    {"desktop", _myDesktop, NULL, SYSTEM, 100, true},
    {"oshello", _osHello, NULL, SYSTEM, 101, true},
    {"form1", _myForm1, _ICON.window_abc, DESKTOP, 0, false},
    {"form2", _myForm2, _ICON.window_shell_1, DESKTOP, 0, false},
    {"form3", _myForm3, _ICON.window_shell_2, DESKTOP, 0, false},
    {"graphics 1", _myGraphicsTest1, _ICON.window_graphics, DESKTOP, 0, false},
    {"graphics 2", _myGraphicsTest2, _ICON.window_graphics, DESKTOP, 0, false},
    {"dispatcher", _myDispatcher, _ICON.app_wizard, DESKTOP, 0, false},
    {"graphics 3", _myGraphicsTest3, _ICON.window_graphics, DESKTOP, 0, false},
    // {"settings", _settingsForm, icon.technical_group, DESKTOP, 0, false},
    // {"userdesktop", _userDesktop, _ICON.program_manager, DESKTOP, 0, false},
    // [!] Last task
    {"cursor", _systemCursor, NULL, SYSTEM, 0, true}
};

// void runExFormStack()
// {
//     if (!formsStack.empty())
//     {
//         exForm *currentForm = formsStack.top();

//         int result = currentForm->showForm();

//         /*
//         The form's "Close" button returns 1
//         */

//         if (result == 1)
//         {
//             formsStack.pop();
//             delete currentForm;

//             delay(250);
//         }
//     }
//     if (formsStack.empty()) _myOSstartupForm();
// }

// void runTasksCore()
// {
//     for (TaskArguments &t : tasks)
//     {
//         if (t.activ)
//         {
//             runExFormStack();
//             t.f();
//         }
//         // runExFormStack();
//     }

//     // systems.executeAllSystemElements();
// }

// void addTasksForSystems()
// {
//     for (TaskArguments &t : system0)
//     {
//         tasks.push_back(t);
//     }
// }