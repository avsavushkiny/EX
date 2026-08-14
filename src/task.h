#pragma once

#include "exForm.h"
#include "taskDispatcher.h"
#include "ui.h"
#include "energySave.h"
#include "dev.h"
#include "ex.h"


extern GGL _GGL;
extern TaskDispatcher _TD;
extern Cursor _CRS;
extern void runExFormStack();
extern FPS _FPS;
extern OTAWebUpdater _OTA_UPDATER;
extern WiFiManager wifiManager;

short _LOAD_CPU{};

/* Form's */
/* Form. Graphics test #1 */
void _graphicsTest1(int xG, int yG, int wG, int hG)
{   
    for (int x = 0; x < 256; x += 2)
    {
        int y = random(12, 150);
        int h = random(150 - y);
        _GGL.gray.drawVLine(x, y, h, _GGL.gray.BLACK, 1); 
    }
}
void _myGraphicsTest1()
{
    exForm *formGraphicsTest1 = new exForm;                   // [0] создали форму
    eGraphics *graphicsTest1 = new eGraphics(_graphicsTest1, 0, 0, 256, 137); // [1] создали элемент формы

    formGraphicsTest1->title = "Graphics test";    // [2] назвали форму
    formGraphicsTest1->eFormShowMode = MAXIMIZED;  // [3] определили режим формы
    formGraphicsTest1->addElement(graphicsTest1);  // [4] добавили эелемент в контейнер

    formsStack.push(formGraphicsTest1); // [5] добавили элемент в стэк форм
}

/* Form. Graphics test #2 */
void _graphicsTest2(int xG, int yG, int wG, int hG)
{ 
    int w{4}, h{4};

    for (int y = yG; y < 150; y += h)
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
    eGraphics *graphicsTest2 = new eGraphics(_graphicsTest2, 0, 0, 256, 137); // [1] создали элемент формы

    formGraphicsTest2->title = "Graphics test 2";  // [2] назвали форму
    formGraphicsTest2->eFormShowMode = MAXIMIZED; // [3] определили режим формы
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
    eGraphics *graphicsTest3 = new eGraphics(_graphicsTest3, 0, 0, 256, 137);

    formGraphicsTest3->title = "Graphics test 3";
    formGraphicsTest3->eFormShowMode = MAXIMIZED;
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
    // _TD.removeTaskIndex(100);
}
/* Form. User Desktop */
void _userDesktop()
{
    exForm *form1 = new exForm();
    eDesktop<TaskArguments> *desktop1 = new eDesktop<TaskArguments>(userTasks);

    form1->title = "User Desktop";
    form1->eFormShowMode = FULLSCREEN;
    form1->addElement(desktop1);

    formsStack.push(form1);
}

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
    formInfoSystems->eFormShowMode = MAXIMIZED;
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
    eButton *buttonReboot = new eButton("Reboot", [](){ ESP.restart(); } , 71, 102);
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

    // Создаём текстовое поле ввода
    eTextInput *textInput = new eTextInput(
        "Enter text:", // метка
        5,             // x
        15,            // y
        120,           // ширина
        20,            // высота
        [](const String &text)
        {
            // Callback при изменении текста
            // Serial.println("Text changed: " + text);
        });

    // Создаём текстовое поле ввода
    eTextInput *textInput2 = new eTextInput(
        "Enter text 2:", // метка
        5,             // x
        47,            // y
        120,           // ширина
        20,            // высота
        [](const String &text)
        {
            // Callback при изменении текста
            // Serial.println("Text changed: " + text);
        });


    // Добавляем кнопку для подтверждения
    eButton *submitBtn = new eButton(
        "Submit",
        []()
        {
            // Получить текст из поля ввода можно через глобальную переменную
            // или через callback
            // Serial.println("Text submitted!");
        },
        130,
        15);

    // Устанавливаем начальный текст
    textInput->setText("Hello World!");
    textInput2->setText("Text 2");

    form1->addElement(submitBtn);
    form1->addElement(textInput);
    form1->addElement(textInput2);

    form1->title = "Form 1. Input text";
    form1->eFormShowMode = NORMAL;

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

    form2->eFormShowMode = MAXIMIZED;

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
/* Form. ADC Monitor with Plotter */
void _adcMonitorForm()
{
    exForm* formADC = new exForm();
    
    eLabel* label = new eLabel("ADC: 0", 0, 5);
    ePlotter* plotter = new ePlotter(5, 20, 246, 90, 500);
    plotter->setYRange(0, 4095);
    
    eButton* clearBtn = new eButton("Clear", [plotter](){
        plotter->clearData();
    }, 5, 115);
    
    eFunction* update = new eFunction([label, plotter](){
        int raw = analogRead(39);
        label->setText("ADC: " + String(raw));
        plotter->addDataPoint(raw);
    });
    
    formADC->addElement(label);
    formADC->addElement(plotter);
    formADC->addElement(clearBtn);
    formADC->addElement(update);
    
    formADC->title = "Plotter. ADC pin 29";
    formADC->eFormShowMode = MAXIMIZED;
    
    formsStack.push(formADC);
}
/* Form. WIFI connect */
void _wifiConnect()
{
    exForm *form = new exForm();
    form->title = "WiFi Connect";
    form->eFormShowMode = MAXIMIZED;

    
    eTextInput *ssidInput = new eTextInput("SSID", 5, 15, 200, 13);
    form->addElement(ssidInput);

    
    eTextInput *passInput = new eTextInput("PASS", 5, 40, 200, 13);
    form->addElement(passInput);
    
    // Статус
    eLabel *statusLabel = new eLabel("Status: Disconnected", 5, 63);
    form->addElement(statusLabel);

    // IP
    eLabel *ipLabel = new eLabel("IP: 0.0.0.0", 5, 73);
    form->addElement(ipLabel);

    
    // Кнопка Connect
    form->addElement(new eButton(
        "Connect",
        [=]() {
            String ssid = ssidInput->getText();
            String pass = passInput->getText();
            
            if (ssid.isEmpty()) {
                statusLabel->setText("Status: Enter SSID!");
                return;
            }
            
            statusLabel->setText("Status: Connecting...");
            
            if (wifiManager.connect(ssid, pass)) {
                statusLabel->setText("Status: Connected!");
                ipLabel->setText("IP: " + wifiManager.getIP().toString());
                wifiManager.saveCredentials(ssid, pass);
            } else {
                statusLabel->setText("Status: Failed!");
            }
        },
        5, 90
    ));

    // Кнопка Disconnect
    form->addElement(new eButton(
        "Disconnect",
        [=]() {
            wifiManager.disconnect();
            statusLabel->setText("Status: Disconnected");
            ipLabel->setText("IP: 0.0.0.0");
        },
        50, 90
    ));

    // Функция обновления статуса
    form->addElement(new eFunction([=]() {
        if (wifiManager.getStatus() == WiFiStatus::CONNECTED) {
            statusLabel->setText("Status: Connected!");
            ipLabel->setText("IP: " + wifiManager.getIP().toString());
        } else if (wifiManager.getStatus() == WiFiStatus::CONNECTING) {
            statusLabel->setText("Status: Connecting...");
        } else if (wifiManager.getStatus() == WiFiStatus::FAILED) {
            statusLabel->setText("Status: Failed!");
        } else {
            statusLabel->setText("Status: Disconnected");
            ipLabel->setText("IP: 0.0.0.0");
        }
    }));

    // Загружаем сохраненные данные
    String savedSSID, savedPass;
    if (wifiManager.loadCredentials(savedSSID, savedPass)) {
        ssidInput->setText(savedSSID);
        passInput->setText(savedPass);
    }

    formsStack.push(form);
}

void wifiAutoReconnect()
{
    wifiManager.autoReconnect();
}


/* Form. Update center*/
bool isOtaMode = false;
void _otaStartUpdater()
{
    if (_OTA_UPDATER.begin("EX-Updater", "12345678"))
    {
        _OTA_UPDATER.startWebServer(80);
        isOtaMode = true;
    }
}

void _otaStopUpdater()
{
    WiFi.mode(WIFI_OFF);
    isOtaMode = false;
}

void _formOTAUpdate()
{
    exForm* formOTAupdate = new exForm();
    
    eTextBox *textBox1 = new eTextBox("You can download the new firmware via\nWi-Fi using a web browser on your phone\nor computer.", BorderStyle::noBorder, 210, 30, 0, 0);
    eButton *button1 = new eButton("Start OTA mode", _otaStartUpdater, 5, 55);
    eButton *button2 = new eButton("Stop OTA mode", _otaStopUpdater, 5, 72);
    eLabel *labelIp = new eLabel("", 0, 40);

    eFunction *funStartOtaUpdate = new eFunction([]() {
        if (isOtaMode == true)
        {
            _OTA_UPDATER.handleClient();
        }
    });

    eFunction *func1 = new eFunction([labelIp](){ 
        if (isOtaMode == true)
        {
            labelIp->setText((String)_OTA_UPDATER.getLocalIP().toString());
        }
        else
        {
            labelIp->setText("0.0.0.0");
        }
    });


    formOTAupdate->title = "Update center";
    formOTAupdate->eFormShowMode = NORMAL;

    formOTAupdate->addElement(textBox1);
    formOTAupdate->addElement(button1);
    formOTAupdate->addElement(button2);
    formOTAupdate->addElement(funStartOtaUpdate);
    formOTAupdate->addElement(func1);
    formOTAupdate->addElement(labelIp);

    formsStack.push(formOTAupdate);
}

/* Cursor */
void _systemCursor()
{
    _JOY.updatePositionXY(10);
    _CRS.cursor(true, _JOY.posX0, _JOY.posY0);

    if (_JOY.posY0 > 132)
    {
        if ((_JOY.pressKeyEX() == true) && (_JOY.pressKeyENTER() == true))
        {
            _GGL.gray.writeLine(_JOY.posX0 + 10, _JOY.posY0, (String)_VERSION_CORE, 10, 1, _GGL.gray.BLACK);
            // _GGL.gray.writeLine(_JOY.posX0 + 10, _JOY.posY0, (String)_DESCRIPTION, 10, 1, _GGL.gray.BLACK);
        }
        else
        {
            // Выводим загрузку CPU
            _GGL.gray.writeLine(_JOY.posX0 + 10, _JOY.posY0, (String)_LOAD_CPU, 10, 1, _GGL.gray.BLACK);
            _FPS.drawGrayFPS(_JOY.posX0 + 23, _JOY.posY0, _GGL.gray.DARK_GRAY);
        }
    }
    else
    {
        if ((_JOY.pressKeyEX() == true) && (_JOY.pressKeyENTER() == true))
        {
            _GGL.gray.writeLine(_JOY.posX0 + 10, _JOY.posY0 + 10, (String)_VERSION_CORE, 10, 1, _GGL.gray.BLACK);
            _GGL.gray.writeLine(_JOY.posX0 + 10, _JOY.posY0 + 20, (String)_DESCRIPTION, 10, 1, _GGL.gray.BLACK);
        }
        else
        {
            // Выводим загрузку CPU
            _GGL.gray.writeLine(_JOY.posX0 + 10, _JOY.posY0 + 10, (String)_LOAD_CPU, 10, 1, _GGL.gray.BLACK);
            _FPS.drawGrayFPS(_JOY.posX0 + 10, _JOY.posY0 + 20, _GGL.gray.DARK_GRAY);
        }
    }
}

/* CPU load */
void monitorTask()
{
    _LOAD_CPU = _TD.getCPULoad();
}

/* Задача с ошибкой */
void testErrorTask()
{
    for(;;)
    {
    }
}

/* Form. Minimized Windows List */
void _minimizedWindowsForm()
{
    exForm *formMinimized = new exForm();
    
    // Контейнер для ссылок на свернутые окна
    // Используем std::vector для хранения ссылок
    std::vector<eLinkLabel*> linkLabels;

    int yPos = 5;      // Переменная для отслеживания позиции Y
    int maxLinks = 10; // Максимальное количество отображаемых окон
    
    // Получаем список свернутых форм
    std::vector<exForm*> minimizedCopy = minimizedForms; // Копируем для безопасного доступа

    eFunction *func1 = new eFunction([](){ 
        
    });
    
    // Если нет свернутых окон
    if (minimizedCopy.empty())
    {
        eLabel *emptyLabel = new eLabel("No minimized windows", 5, yPos);
        formMinimized->addElement(emptyLabel);
    }
    else
    {
        // Создаем ссылки для каждого свернутого окна
        for (size_t i = 0; i < minimizedCopy.size() && i < maxLinks; i++)
        {
            exForm* form = minimizedCopy[i];
            if (form == nullptr) continue;
            
            // Создаем ссылку с именем формы
            String linkText = "- " + form->title;
            
            // Создаем eLinkLabel с функцией восстановления
            eLinkLabel *link = new eLinkLabel(
                linkText,
                [form]() {
                    // Восстанавливаем форму
                    restoreForm(form);
                    // Обновляем стек форм
                    // Форма будет показана при следующем обновлении
                },
                5,
                yPos
            );
            
            linkLabels.push_back(link);
            formMinimized->addElement(link);
            
            yPos += 10; // Отступ между элементами
        }
        
        // Если окон больше чем maxLinks, показываем индикатор
        if (minimizedCopy.size() > maxLinks)
        {
            eLabel *moreLabel = new eLabel(
                "... and " + String(minimizedCopy.size() - maxLinks) + " more",
                5,
                yPos
            );
            formMinimized->addElement(moreLabel);
        }
    }
    
    // Кнопка "Закрыть все"
    eButton *closeAllBtn = new eButton(
        "Close All",
        []() {
            // Закрываем все свернутые окна
            std::vector<exForm*> copy = minimizedForms;
            for (exForm* form : copy)
            {
                if (form != nullptr)
                {
                    // Удаляем из списка свернутых
                    auto it = std::find(minimizedForms.begin(), minimizedForms.end(), form);
                    if (it != minimizedForms.end())
                    {
                        minimizedForms.erase(it);
                    }
                    delete form;
                }
            }
        },
        5,
        120
    );
    
    formMinimized->title = "Minimized Windows";
    formMinimized->eFormShowMode = MAXIMIZED;

    
    formMinimized->addElement(closeAllBtn);
    formMinimized->addElement(func1);
    
    formsStack.push(formMinimized);
}

/* Event update */
void eventTask()
{
    Events.update();
}

/* Test */
void eventTest()
{
    static unsigned long lastPressTime = 0;
    static int clickCount = 0;
    static bool waitingForSecond = false;
    static bool isLongPress = false;
    static unsigned long pressStartTime = 0;
    static bool buttonPressed = false;  // Добавляем флаг состояния кнопки
    
    // Проверка нажатия кнопки ENTER
    bool isPressed = _JOY.pressKeyENTER();
    
    // Обработка нажатия (переход из отпущена в нажата)
    if (isPressed && !buttonPressed)
    {
        buttonPressed = true;
        pressStartTime = millis();
        
        // Проверяем, не было ли это вторым кликом в двойном клике
        if (waitingForSecond && (millis() - lastPressTime) <= 300)
        {
            clickCount++;
            if (clickCount >= 2)
            {
                Serial.println("DOUBLE CLICK!");
                // Действие при двойном клике
                clickCount = 0;
                waitingForSecond = false;
                pressStartTime = 0;
            }
        }
        else
        {
            // Первый клик или таймаут
            clickCount = 1;
            waitingForSecond = true;
            lastPressTime = millis();
            Serial.println("First click");
        }
    }
    // Обработка отпускания (переход из нажата в отпущена)
    else if (!isPressed && buttonPressed)
    {
        buttonPressed = false;
        unsigned long pressDuration = millis() - pressStartTime;
        
        // Проверка долгого нажатия
        if (pressDuration >= 500)
        {
            if (!isLongPress)
            {
                isLongPress = true;
                Serial.println("LONG PRESS!");
                // Действие при долгом нажатии
                clickCount = 0;
                waitingForSecond = false;
            }
        }
        
        pressStartTime = 0;
    }
    // Обработка удержания (кнопка нажата)
    else if (isPressed && buttonPressed)
    {
        unsigned long pressDuration = millis() - pressStartTime;
        
        // Проверка долгого нажатия во время удержания
        if (pressDuration >= 500 && !isLongPress)
        {
            isLongPress = true;
            Serial.println("LONG PRESS!");
            // Действие при долгом нажатии
            clickCount = 0;
            waitingForSecond = false;
        }
    }
    
    // Таймаут для одиночного клика
    if (waitingForSecond && clickCount == 1 && (millis() - lastPressTime) > 300)
    {
        Serial.println("Single click");
        // Действие при одиночном клике
        waitingForSecond = false;
        clickCount = 0;
        isLongPress = false;
    }
    
    // Сброс состояния после долгого нажатия
    if (isLongPress && !isPressed)
    {
        isLongPress = false;
        clickCount = 0;
        waitingForSecond = false;
    }
}

/**/
// Вспомогательная функция для создания задач с параметрами по умолчанию
TaskArguments createTask(String name, void (*f)(void), const uint8_t *bitMap, 
                        TaskType type, int index, bool activ, 
                        TaskPriority priority = PRIORITY_NORMAL, 
                        bool oneShot = false, unsigned long interval = 1)
{
    TaskArguments task;
    task.name = name;
    task.f = f;
    task.bitMap = bitMap;
    task.type = type;
    task.index = index;
    task.activ = activ;
    task.priority = priority;
    task.oneShot = oneShot;
    task.interval = interval;
    task.lastRunTime = 0;
    task.nextRunTime = 0;
    return task;
}

/* Tasklist */
TaskArguments system0[]{
    //        (название, функция, bitmap, тип, индекс, статус, ПРИОРИТЕТ, oneshot, тик)
    /* Рабочий стол */
    createTask("desktop", &_myDesktop, NULL, SYSTEM, 100, true, PRIORITY_NORMAL, true, 1),
    /* Инициализация таймера */
    createTask("initSleepTimerTask", &initSleepTimerTask, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, true, 1),
    /* Энергосбережение */
    createTask("energySave", &energySave, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, 0, 100), // было 10
    /* Обновление состояния кнопок (высокий приоритет, часто) */
    createTask("eventUpdate", &eventTask, NULL, SYSTEM, 0, true, PRIORITY_HIGH, false, 10),
    /* Диспетчер свернутых форм */
    createTask("Minimized Windows", &_minimizedWindowsForm, _ICON.program_manager, DESKTOP, 0, false, PRIORITY_NORMAL),
    // createTask("oshello", &_osHello, NULL, SYSTEM, 101, true, PRIORITY_NORMAL),
    createTask("Form 1", &_myForm1, _ICON.window_abc, DESKTOP, 0, false, PRIORITY_NORMAL),
    createTask("Form 2", &_myForm2, _ICON.window_shell_1, DESKTOP, 0, false, PRIORITY_NORMAL),
    createTask("Form 3", &_myForm3, _ICON.window_shell_2, DESKTOP, 0, false, PRIORITY_NORMAL),
    createTask("Graphics 1", &_myGraphicsTest1, _ICON.window_graphics, DESKTOP, 0, false, PRIORITY_NORMAL),
    createTask("Graphics 2", &_myGraphicsTest2, _ICON.window_graphics, DESKTOP, 0, false, PRIORITY_NORMAL),
    createTask("graphics 3", &_myGraphicsTest3, _ICON.window_graphics, DESKTOP, 0, false, PRIORITY_NORMAL),

    // 
    createTask("WiFi", &_wifiConnect, _ICON.window_shell_1, DESKTOP, 0, false, PRIORITY_NORMAL),
    //
    // createTask("wifiAuto", &wifiAutoReconnect, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, false, 100),

    
    createTask("ADC Monitor", &_adcMonitorForm, _ICON.window_graphics, DESKTOP, 0, false, PRIORITY_NORMAL),
    // User
    createTask("User", &_userDesktop, _ICON.computer, DESKTOP, 0, false, PRIORITY_NORMAL),
    // Error task
    createTask("Error", &testErrorTask, _ICON.chip_ram, DESKTOP, 0, false, PRIORITY_NORMAL),
    // OTA update
    createTask("Update centre", &_formOTAUpdate, _ICON.technical_group, DESKTOP, 0, false, PRIORITY_NORMAL),
    // Stack forms
    createTask("stackform", &runExFormStack, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, false, 1),
    // Добавление задачи мониторинга
    createTask("monitor", &monitorTask, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, false, 100),
    // Cursor
    createTask("cursor", &_systemCursor, NULL, SYSTEM, 0, true, PRIORITY_LOW, false, 10) // было 1
};

/*
Одноразовая задача:
createTask("init", &initFunction, NULL, SYSTEM, 0, true, PRIORITY_HIGH, true)

Периодическая задача с интервалом:
createTask("sensor", &readSensor, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, false, 10) // выполняется каждые 10 тиков

Критическая задача:
createTask("emergency", &emergencyHandler, NULL, SYSTEM, 0, true, PRIORITY_CRITICAL, false, 1)
*/