/*
  Version OS 0.1.5 with 'ggl' graphic library
  Dev: Aleksander Savushkin
  06/03/2025
*/

/*
  Contains function settings for working with the display.

  [!] Required u8g2 library
  [!] bmp to xbmp image converter https://www.online-utility.org/image/convert/to/XBM
                                  https://windows87.github.io/xbm-viewer-converter/
  [!] midi to arduino tones converter https://arduinomidi.netlify.app/
      ntp client                  https://github.com/arduino-libraries/NTPClient
  [!] u8g2 library reference      https://github.com/olikraus/u8g2/wiki/u8g2reference

*/

#include <iostream>
#include <Arduino.h>
#include <U8g2lib.h> 
#include "ex.h"
#include "ex_xbm.h"
#include "ggl.h"

#include <vector>
#include <functional>
#include <algorithm>

//version Library and Text
const int8_t VERSION_LIB[] = {0, 1, 5};
const int8_t VERSION_GGL[] = {0, 1, 1};

GGL ggl; SystemIcon sysIcon; FPS fps; Icon icon;

Graphics _gfx; 
Timer _delayCursor, _trm0, _trm1, _stop, _timerUpdateClock, _fps; 
 
Joystick _joy; 
Shortcut _shortcutDesktop, _wifi;
Cursor _crs; 
PowerSave _pwsDeep; 
Interface _mess; 
Button _ok, _no, _collapse, _expand, _close, _ledControl;

Label _labelClock, _labelBattery, _labelWifi, _taskList;
TextBox _textBox;

TaskDispatcher td; TextBuffer textBuffer;

/* System task */
// Устанавливаем значения
Systems systems(false, 143, true, true, false, 1);

/* Stack exForm */
std::stack<exForm*> formsStack;

/* LED control */
bool systemStateLedControl = true; bool flagStateLedControl = false;

/* Prototype function */
void nullFunction(); void _rebootBoard();
//vector only
void addTasksForSystems();

//for screensaver
unsigned long screenTiming{}, screenTiming2{}, TIMER{};

//for classes-timer
unsigned long previousMillis{};
unsigned long prevTime_0{};
const long interval{300};

/* Liquid crystal display resolution. */
int H_LCD{160}, W_LCD{256};

/* Global coordinates */
int GLOBAL_X{0}, GLOBAL_Y{0};

/* Analog-to-digital converter resolution (Chip ESP32). */
const int8_t RESOLUTION_ADC{12};

/* Port data. */
const int8_t PIN_STICK_0X = 33;  // adc 
const int8_t PIN_STICK_0Y = 32;  // adc 

const int8_t PIN_STICK_1Y = 34;  // adc 
const int8_t PIN_STICK_1X = 35;  // adc 

const int8_t PIN_BUTTON_ENTER = 27;  // gp 
const int8_t PIN_BUTTON_EX    = 14;  // gp 
const int8_t PIN_BUTTON_A     = 12;  // gp
const int8_t PIN_BUTTON_B     = 13;  // gp

const int8_t PIN_BACKLIGHT_LCD = 0;  // gp 
const int8_t PIN_BUZZER  = 26;       // gp 
const int8_t PIN_BATTERY = 39;       // gp


void db(String text)
{
    Serial.println(text);
}
void db(int value)
{
    Serial.println(value);
}
/* 
    class
    Graphics
*/
/* system initialization */
void Graphics::initializationSystem()
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
    ggl.gray.begin();

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
    systems.setDisplayContrast(240);
    systems.executeAllSystemElements();
    

    /*
       Vector
       moving system-task to the vector
       determine the number of tasks in the vector
    */
    addTasksForSystems();

    // Clear buffer LCD display
    ggl.gray.clearBuffer(); // -->

    // output text
    String text = "SozvezdiyeOs + GGL";
    ggl.gray.writeLine(5, 123, (String)VERSION_LIB[0] + "." + (String)VERSION_LIB[1] + "." + (String)VERSION_LIB[2] + " " + text, 10, 1, ggl.gray.BLACK);
    
    // draw gray-line
    ggl.gray.drawHLine(0, 135, 256, ggl.gray.LIGHT_GRAY, 5);
    ggl.gray.drawHLine(0, 140, 256, ggl.gray.DARK_GRAY, 5);
    ggl.gray.drawHLine(0, 145, 256, ggl.gray.BLACK, 5);

    // send data to display
    ggl.gray.sendBuffer(); // <--

    delay(2500);
}
/* data render (full frame) */
void Graphics::render(void (*f)(), int timeDelay)
{
    uint32_t time;

    time = millis() + timeDelay;

    do
    {
        ggl.gray.clearBuffer();
        f();
        ggl.gray.sendBuffer();
    } while (millis() < time);
}
/* data render (full frame) no time delay */
void Graphics::render(void (*f)())
{
    ggl.gray.clearBuffer();
    fps.updateFPS(); fps.drawGrayFPS(2, 150, ggl.gray.LIGHT_GRAY);
    f();
    ggl.gray.sendBuffer();
}
/* data render two function (full frame) */
void Graphics::render(void (*f1)(), void (*f2)())
{
    ggl.gray.clearBuffer();
    f1();
    f2();
    ggl.gray.sendBuffer();
}
/* clearing the output buffer */
void Graphics::clear()
{
    ggl.gray.clearBuffer(); // clear buffer, add 0x00
    ggl.gray.sendBuffer();  // send data to buffer display
}
/* text output with parameters, add line interval (def: 10) and character interval (def: 6) */
/* text, x-position, y-position, line interval (8-10), character interval (4-6) */ 
void Graphics::print(String text, int x, int y, int8_t lii, int8_t chi)
{
    int sizeText = text.length();
    int yy{0};

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        ggl.gray.writeChar(xx + x, yy + y, text[i], 10, 1, ggl.gray.BLACK);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}
/* wait display */
bool Graphics::waitDisplay()
{
    ggl.gray.clearBuffer(); // -->
    ggl.gray.bitmap(((W_LCD - sysIcon.wait_w)/2), ((H_LCD - sysIcon.wait_h)/2), sysIcon.wait, sysIcon.wait_w, sysIcon.wait_h, ggl.gray.NOT_TRANSPARENT);
    ggl.gray.sendBuffer();  // <--
    delay(150); return true;
}


/* 
    class
    Cursor
*/
/* displaying the cursor on the screen */
bool Cursor::cursor(bool stateCursor, int xCursor, int yCursor)
{
    if (stateCursor == true)
    {

        ggl.gray.bitmap(xCursor, yCursor, sysIcon.cursor0, sysIcon.cursor0_w, sysIcon.cursor0_h, ggl.gray.TRANSPARENT);
        
        return true;
    }
    else
        return false;
}


/* 
    class
    Button
*/
/* button return boolean state */
bool Button::button2(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor)
{
  uint8_t sizeText = text.length(); short border{3}; short charW{5};

  if ((xCursor >= x && xCursor <= (x + (sizeText * charW) + border + border)) && (yCursor >= y && yCursor <= y + 13))
  {
    ggl.gray.drawFillFrame(x, y, (sizeText * charW) + border + border, 13, ggl.gray.BLACK, ggl.gray.BLACK);
    ggl.gray.writeLine(x + border, y - 1/* font H */ + border, text, 10, 1, ggl.gray.WHITE);

    if (Joystick::pressKeyENTER() == true)
    {
      return true;
    }
  }
  else
  {
    ggl.gray.drawFillFrame(x, y, (sizeText * charW) + border + border, 13, ggl.gray.BLACK, ggl.gray.WHITE);
    ggl.gray.writeLine(x + border, y - 1/* font H */ + border, text, 10, 1, ggl.gray.BLACK);
  }
  
  return false;
}


/* 
    class
    Shortcut
*/
/* displaying a shortcut to a task-function */
bool Shortcut::shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  ggl.gray.bitmap(x, y, bitMap, 32, 32, ggl.gray.NOT_TRANSPARENT);

  ggl.gray.bitmap(x, y + 21, sysIcon.shortcut0, 11, 11, ggl.gray.NOT_TRANSPARENT);

  TextBox textBoxNameTask;

  if ((xCursor >= x && xCursor <= (x + 32)) && (yCursor >= y && yCursor <= (y + 32)))
  {
    ggl.gray.drawFrame(x, y, 32, 32, ggl.gray.BLACK);

    textBoxNameTask.textBox(name, 16, 32, 8, 5, x, y + 24);
    
    if (Joystick::pressKeyENTER() == true)
    {
      _gfx.waitDisplay();
      f();
      return true;
    }
  }
  else
  {
    textBoxNameTask.textBox(name, 16, 32, 8, 5, x, y + 24);
  }

  return false;
}


/* 
     class
     TextBox
*/
/* [for Desktop name tasks] no Frame */
void TextBox::textBox(String str, int sizeH, int sizeW, short charH, short charW, int x, int y)
{
    short border{0}; short border2{0}; // size border
    int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
    int line{1}; // there will always be at least one line of text in the text
    int numberOfCharacters{0}; // количество символов
    int ch{0}, ln{0}; int xx{x}, yy{y};  
    
    for (char c : str)
    {
        numberOfCharacters++;
    }

    int numberOfCharactersLineFrame = (sizeW - border - border) / charW; // количество символов в строчке Фрейма
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
    int numberOfLinesFrame = (sizeH - border - border) / charH; // количество строчек в Фрейме
    
    for (char c : str)
    {
        ggl.gray.writeChar(xx + border, yy + charH + border, c, 10, 1, ggl.gray.BLACK);
        
        xx += charW;
        ch++;

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH; ch = 0; xx = x; ln++;
        }

        if (ln >= numberOfLinesFrame)
        {
            break;
        }
    }
}


/* 
    class
    Joystic and Key
*/
/* button control */
bool Joystick::pressKeyENTER()
{
    if (digitalRead(PIN_BUTTON_ENTER) == true)
    {
        EVENT_KEY = KEY_PRESSED_ENTER;
        return true;
    }
    else
        EVENT_KEY = KEY_NOT_PRESSED;
        return false;
}
/* button control */
bool Joystick::pressKeyEX()
{
    if (digitalRead(PIN_BUTTON_EX) == true)
    {
        EVENT_KEY = KEY_PRESSED_EX;
        return true;
    }
    else
        EVENT_KEY = KEY_NOT_PRESSED;
        return false;
}
/* button control */
bool Joystick::pressKeyA()
{
    if (digitalRead(PIN_BUTTON_A) == true)
    {
        EVENT_KEY = KEY_PRESSED_A;
        return true;
    }
    else
        EVENT_KEY = KEY_NOT_PRESSED;
        return false;
}
/* button control */
bool Joystick::pressKeyB()
{
    if (digitalRead(PIN_BUTTON_B) == true)
    {
        EVENT_KEY = KEY_PRESSED_B;
        return true;
    }
    else
        EVENT_KEY = KEY_NOT_PRESSED;
        return false;
}
/* calculate Stick position */
int Joystick::calculatePositionY0() // 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 600)) /*&& (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100))*/)
    {
        COOR_Y0 += 2;
        if (COOR_Y0 >= 160) COOR_Y0 = 160;
        EVENT_JOYSTICK = JOY_ACTIVE_Y;
        return COOR_Y0;
    }
    /*else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        COOR_Y0 -= 2;
        if (COOR_Y0 <= 0) COOR_Y0 = 160;
        return COOR_Y0;
    }*/
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 600)) /*&& (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100))*/)
    {
        COOR_Y0 -= 2;
        if (COOR_Y0 <= 0) COOR_Y0 = 0;
        EVENT_JOYSTICK = JOY_ACTIVE_Y;
        return COOR_Y0;
    }
   /*else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        COOR_Y0 += 2;
        if (COOR_Y0 >= 160) COOR_Y0 = 0;
        return COOR_Y0;
    }*/
    else
        EVENT_JOYSTICK = JOY_NOT_ACTIVE_Y;
        return COOR_Y0;
}
/* calculate Stick position */
int Joystick::calculatePositionY1() // 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 500)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        COOR_Y1 -= 1;
        if (COOR_Y1 <= 0) COOR_Y1 = 160;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        COOR_Y1 -= 2;
        if (COOR_Y1 <= 0) COOR_Y1 = 160;
        return COOR_Y1;
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 500)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        COOR_Y1 += 1;
        if (COOR_Y1 >= 160) COOR_Y1 = 0;
        return COOR_Y1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        COOR_Y1 += 2;
        if (COOR_Y1 >= 160) COOR_Y1 = 0;
        return COOR_Y1;
    }
    else
        return COOR_Y1;
}
/* calculate Stick position */
int Joystick::calculatePositionX0() // 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 600)) /*&& (RAW_DATA_X0 > (DEF_RES_X0 - 1200))*/)
    {
        COOR_X0 -= 2;
        if (COOR_X0 <= 0) COOR_X0 = 0;
        EVENT_JOYSTICK = JOY_ACTIVE_X;
        return COOR_X0; 
    }
    /*else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        COOR_X0 += 2;
        if (COOR_X0 >= 256) COOR_X0 = 0;
        return COOR_X0;

    }*/
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 600)) /*&& (RAW_DATA_X0 < (DEF_RES_X0 + 1100))*/)
    {
        COOR_X0 += 2;
        if (COOR_X0 >= 256) COOR_X0 = 256;
        EVENT_JOYSTICK = JOY_ACTIVE_X;
        return COOR_X0; 
    }
    /*else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        COOR_X0 -= 2;
        if (COOR_X0 <= 0) COOR_X0 = 256;
        return COOR_X0;
    }*/
    else
        EVENT_JOYSTICK = JOY_NOT_ACTIVE_X;
        return COOR_X0;
}
/* calculate Stick position */
int Joystick::calculatePositionX1() // 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 500)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        COOR_X1 += 1;
        if (COOR_X1 >= 256) COOR_X1 = 0;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        COOR_X1 += 2;
        if (COOR_X1 >= 256) COOR_X1 = 0;
        return COOR_X1;
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 500)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        COOR_X1 -= 1;
        if (COOR_X1 <= 0) COOR_X1 = 256;
        return COOR_X1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        COOR_X1 -= 2;
        if (COOR_X1 <= 0) COOR_X1 = 256;
        return COOR_X1;
    }
    else
        return COOR_X1;
}
/* updating Stick coordinates */
void Joystick::updatePositionXY()
{
    posX0 = calculatePositionX0(); //
    posX1 = calculatePositionX1();
    posY0 = calculatePositionY0(); //
    posY1 = calculatePositionY1();

    indexX0 = calculateIndexX0();
    indexX1 = calculateIndexX1();
    indexY0 = calculateIndexY0();
    indexY1 = calculateIndexY1();
}
/* resetting the cursor position */
void Joystick::resetPositionXY()
{
    COOR_X0 = 128; COOR_X1 = 128; COOR_Y0 = 80; COOR_Y1 = 80;
}
/* updating Stick coordinates */
void Joystick::updatePositionXY(uint delay)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= delay)
    {
        prevTime = currTime;

        posX0 = calculatePositionX0(); //
        posX1 = calculatePositionX1();
        posY0 = calculatePositionY0(); //
        posY1 = calculatePositionY1();

        indexX0 = calculateIndexX0();
        indexX1 = calculateIndexX1();
        indexY0 = calculateIndexY0();
        indexY1 = calculateIndexY1();
    }
}
/* calculate position index */
int8_t Joystick::calculateIndexY0() // obj 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 500)) && (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100)))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_Y;
        return OBJ_Y0 = OBJ_Y0 - 1;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_Y;
        return OBJ_Y0 = OBJ_Y0 - 1; // 2
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 500)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_Y;
        return OBJ_Y0 = OBJ_Y0 + 1;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_Y;
        return OBJ_Y0 = OBJ_Y0 + 1; // 2
    }
    else
        EVENT_JOYSTICK = JOY_NOT_ACTIVE_Y;
        return OBJ_Y0 = 0;
}
/* calculate position index */
int8_t Joystick::calculateIndexY1() // obj 1y
{
    RAW_DATA_Y1 = analogRead(PIN_STICK_1Y);

    if ((RAW_DATA_Y1 < (DEF_RES_Y1 - 500)) && (RAW_DATA_Y1 > (DEF_RES_Y1 - 1100)))
    {
        return OBJ_Y1 = OBJ_Y1 - 1;
    }
    else if (RAW_DATA_Y1 < (DEF_RES_Y1 - 1100))
    {
        return OBJ_Y1 = OBJ_Y1 - 1; // 2
    }
    else if ((RAW_DATA_Y1 > (DEF_RES_Y1 + 500)) && (RAW_DATA_Y1 < (DEF_RES_Y1 + 1100)))
    {
        return OBJ_Y1 = OBJ_Y1 + 1;
    }
    else if (RAW_DATA_Y1 > (DEF_RES_Y1 + 1100))
    {
        return OBJ_Y1 = OBJ_Y1 + 1; // 2
    }
    else
        return OBJ_Y1 = 0;
}
/* calculate position index */
int8_t Joystick::calculateIndexX0() // obj 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 500)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_X;
        return OBJ_X0 = OBJ_X0 - 1;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_X;
        return OBJ_X0 = OBJ_X0 - 1; // 2
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 500)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_X;
        return OBJ_X0 = OBJ_X0 + 1;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        EVENT_JOYSTICK = JOY_ACTIVE_X;
        return OBJ_X0 = OBJ_X0 + 1; // 2
    }
    else
        EVENT_JOYSTICK = JOY_NOT_ACTIVE_X;
        return OBJ_X0 = 0;
}
/* calculate position index */
int8_t Joystick::calculateIndexX1() // obj 1x
{
    RAW_DATA_X1 = analogRead(PIN_STICK_1X);

    if ((RAW_DATA_X1 < (DEF_RES_X1 - 500)) && (RAW_DATA_X1 > (DEF_RES_X1 - 1100)))
    {
        return OBJ_X1 = OBJ_X1 - 1;
    }
    else if (RAW_DATA_X1 < (DEF_RES_X1 - 1100))
    {
        return OBJ_X1 = OBJ_X1 - 1; // 2
    }
    else if ((RAW_DATA_X1 > (DEF_RES_X1 + 500)) && (RAW_DATA_X1 < (DEF_RES_X1 + 1100)))
    {
        return OBJ_X1 = OBJ_X1 + 1;
    }
    else if (RAW_DATA_X1 > (DEF_RES_X1 + 1100))
    {
        return OBJ_X1 = OBJ_X1 + 1; // 2
    }
    else
        return OBJ_X1 = 0;
}


/* 
    class
    Timer
*/
/* starting a task-function with an interval */
void Timer::timer(void (*f)(void), int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        f();
    }
}
/* starting a task-function with an interval */
void Timer::timer(int (*f)(void), int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        f();
    }
}
/* return value */
bool Timer::timer(int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        return 1;
    }
    return 0;
}


/* Null function */
void nullFunction(){}

/*
    eForm
    Visual eForm Builder
    [01/2025, Alexander Savushkin]
*/
/* eButton */
void eButton::show()
{
    uint8_t sizeText = m_label.length();
    short border{3};
    short charW{5};

    // int x{m_x}, y{m_y + 6 /* offset by Y */};

    // u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    // u8g2.setBitmapMode(0);// 0-off, 1-on Transporent mode
    // u8g2.setColorIndex(1);// 0-off px, 1-on px

    if ((_joy.posX0 >= xForm && _joy.posX0 <= (xForm + (sizeText * charW) + border + border)) && (_joy.posY0 >= yForm && _joy.posY0 <= yForm + 13))
    {

        // u8g2.setColorIndex(1);                                               // включаем пиксели
        // u8g2.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13);    // рисуем черный бокс
        // u8g2.setColorIndex(0);                                               // отключаем птксели
        // _gfx.print(m_label, xForm + border, yForm + 7 /* font H */ + border, 8, charW); // выводим тест
        // u8g2.setColorIndex(1);                                               // включаем пиксели

        // ggl.gray.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13, ggl.gray.DARK_GRAY);
        ggl.gray.drawFillFrame(xForm, yForm, (sizeText * charW) + border + border, 13, ggl.gray.BLACK, ggl.gray.DARK_GRAY);
        ggl.gray.writeLine(xForm + border, yForm - 1/* font H */ + border, m_label, 10, 1, ggl.gray.BLACK);

        if (_joy.pressKeyENTER() == true)
        {
            ggl.gray.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13, ggl.gray.BLACK);
            m_stateButton = true;
            m_func(); 
        }
        else m_stateButton = false;
    }
    else
    {
        // u8g2.setColorIndex(1);                                                  // включаем пиксели
        // u8g2.drawFrame(xForm, yForm, (sizeText * charW) + border + border, 13);     // рисуем прозрачный фрейм
        // _gfx.print(m_label, xForm + border, yForm + 7 /* font H */ + border, 8, charW); // выводим тест

        ggl.gray.drawFillFrame(xForm, yForm, (sizeText * charW) + border + border, 13, ggl.gray.BLACK, ggl.gray.WHITE);
        ggl.gray.writeLine(xForm + border, yForm - 1 /* font H */ + border, m_label, 10, 1, ggl.gray.BLACK);
    }
}
/* eText */
void eText::show()
{
    _gfx.print(m_text, xForm, yForm, 10, 5);
    // ggl.gray.writeLine(xForm, yForm, m_text, 10, 1, ggl.gray.BLACK);
}
/* eTextbox */
void eTextBox::show()
{
    short border{5}; short border2{8}; // size border
    int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
    short line{1}; // there will always be at least one line of text in the text
    int numberOfCharacters{0}; // количество символов
    short charH{10}, charW{5};
    int ch{0}, ln{0}; int xx{xForm}, yy{yForm}; 
    
    if (m_borderStyle == noBorder){ /* we don't draw anything */ }
    if (m_borderStyle == oneLine)
    {
        // u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
        ggl.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, ggl.gray.BLACK);
    }
    if (m_borderStyle == twoLine)
    {
        // u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
        // u8g2.drawFrame(xx - 3, yy - 3, m_sizeW + 6, m_sizeH + 6);

        ggl.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, ggl.gray.BLACK);
        ggl.gray.drawFrame(xx - 3, yy - 3, m_sizeW + 6, m_sizeH + 6, ggl.gray.BLACK);
    }
    if (m_borderStyle == shadow)
    {
        // u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
        ggl.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, ggl.gray.BLACK);

        // u8g2.drawHLine(xx + 1, yy + m_sizeH, m_sizeW);
        // u8g2.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW);

        ggl.gray.drawHLine(xx + 1, yy + m_sizeH, m_sizeW, ggl.gray.BLACK, 1);
        ggl.gray.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW, ggl.gray.BLACK, 1);

        // u8g2.drawVLine(xx + m_sizeW, yy + 1, m_sizeH);
        // u8g2.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH);

        ggl.gray.drawVLine(xx + m_sizeW, yy + 1, m_sizeH, ggl.gray.BLACK, 1);
        ggl.gray.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH, ggl.gray.BLACK, 1);
    }
    if (m_borderStyle == shadowNoFrame)
    {
        // u8g2.drawHLine(xx + 1, yy + m_sizeH, m_sizeW);
        // u8g2.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW);

        ggl.gray.drawHLine(xx + 1, yy + m_sizeH, m_sizeW, ggl.gray.BLACK, 1);
        ggl.gray.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW, ggl.gray.BLACK, 1);

        // u8g2.drawVLine(xx + m_sizeW, yy + 1, m_sizeH);
        // u8g2.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH);

        ggl.gray.drawVLine(xx + m_sizeW, yy + 1, m_sizeH, ggl.gray.BLACK, 1);
        ggl.gray.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH, ggl.gray.BLACK, 1);
    }
    
    
    for (char c : m_text)
    {
        numberOfCharacters++;
    }

    int numberOfCharactersLineFrame = (m_sizeW - border - border) / charW; // количество символов в строчке Фрейма
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
    int numberOfLinesFrame = (m_sizeH - border - border) / charH; // количество строчек в Фрейме
    
    for (char c : m_text)
    {
        // u8g2.setFont(u8g2_font_6x10_tr);
        // u8g2.setCursor(xx + border, yy + charH + border);
        // u8g2.print(c);

        // ggl.gray.writeLine(xx + border, yy + charH + border, c, 10, 1, ggl.gray.BLACK);
        ggl.gray.writeChar(xx + border, yy + border, c, 10, 1, ggl.gray.BLACK);

        // adds dots if frame is full
        /*if ((ln >= numberOfLinesFrame - 1) && (numberOfCharactersLineFrame == (ch + 3)))
        {
            u8g2.print("...");
        }
        else u8g2.print(c);*/
        
        xx += charW;
        ch++;

        if (c == '\n')
        {
            yy += charH; ch = 0; xx = xForm; ln++;
        }

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH; ch = 0; xx = xForm; ln++;
        }

        // if (ln >= numberOfLinesFrame)
        // {
        //     // draw glyph
        //     /*u8g2.setFont(u8g2_font_unifont_t_symbols);
        //     u8g2.drawGlyph(x + sizeW - 8, y + sizeH + 2, 0x2198);*/

        //     u8g2.drawTriangle(xx + m_sizeW, yy + m_sizeH - 8, xx + m_sizeW, yy + m_sizeH, xx + m_sizeW - 8, yy + m_sizeH);
        //     break;
        // }
    }
}
/* eLabel */
void eLabel::show()
{
    uint8_t sizeText = m_text.length();
    uint8_t chi{5}, lii{8};
    int x{xForm}, y{yForm};

    // u8g2.setColorIndex(1); // включаем пиксели

    // u8g2.setCursor(x, y);
    // u8g2.setFont(u8g2_font_6x10_tr);

    for (int i = 0; i < sizeText; i++)
    {
        // u8g2.setCursor(x, y);
        // u8g2.print(m_text[i]); 
        x += chi;
        ggl.gray.writeChar(x, y, m_text[i], 10, 1, ggl.gray.BLACK);

        if (m_text[i] == '\n')
        {
            x = xForm; y += lii;
        }
    }
}
/* eLabel with a link */
void eLinkLabel::show()
{
    uint8_t sizeText = m_text.length();
    uint8_t yy{}, chi{5}, lii{8}; int x{xForm + 1}, y{yForm};

    if ((_joy.posX0 >= x && _joy.posX0 <= (x + (sizeText * chi))) && (_joy.posY0 >= y - 2 && _joy.posY0 <= y + lii + 2))
    {

        // u8g2.setColorIndex(1);                                         // включаем пиксели
        // u8g2.drawBox(x - 1, y - (lii), (sizeText * chi) + 2, lii + 1); // рисуем черный бокс
        ggl.gray.drawBox(x - 1, y, (sizeText * chi) + 2, lii + 1, ggl.gray.BLACK);
        // u8g2.setColorIndex(0);                                         // отключаем пиксели

        // u8g2.setCursor(x + 3, y);
        // u8g2.setFont(u8g2_font_6x10_tr);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            // u8g2.setCursor(xx + x, yy + y);
            // u8g2.print(m_text[i]);
            ggl.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, ggl.gray.WHITE);

            if (m_text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
        // u8g2.setColorIndex(1);                                          // включаем пиксели


        if (_joy.pressKeyENTER() == true)
        {
            for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
            {
                // u8g2.setCursor(xx + x, yy + y);
                // u8g2.print(m_text[i]);
                ggl.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, ggl.gray.DARK_GRAY);
    
                if (m_text[i] == '\n')
                {
                    yy += lii; // 10
                    xx = -chi; // 6
                }
            }
            
            m_onClick();
        }
    }
    else
    {
        // u8g2.setColorIndex(1);                                          // включаем пиксели

        // u8g2.setCursor(x + 3, y);
        // x = x + 3;
        // u8g2.setFont(u8g2_font_6x10_tr);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            // u8g2.setCursor(xx + x, yy + y);
            // u8g2.print(m_text[i]);
            ggl.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, ggl.gray.BLACK);

            if (m_text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
    }
}
/* Horizontal line */
void eLine::show()
{
    // u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    // u8g2.setBitmapMode(0);// 0-off, 1-on Transporent mode
    // u8g2.setColorIndex(1);// 0-off px, 1-on px
    // u8g2.drawHLine(xForm, yForm, wForm);
    ggl.gray.drawHLine(xForm, yForm, wForm, ggl.gray.BLACK, 1);
}
/* eCheckbox */
void eCheckbox::show()
{
    // рисуем фрейм и выводим текст
    if (m_checked == true)
    {
        // u8g2.drawXBMP(xForm, yForm, check_true_w, check_true_h, check_true);
        ggl.gray.drawFillFrame(xForm, yForm, 10, 10, ggl.gray.BLACK, ggl.gray.DARK_GRAY);
    }

    if (m_checked == false)
    {
        // u8g2.drawXBMP(xForm, yForm, check_false_w, check_false_h, check_false);
        ggl.gray.drawFillFrame(xForm, yForm, 10, 10, ggl.gray.BLACK, ggl.gray.LIGHT_GRAY);
    }

    // _gfx.print(m_text, xForm + 15, yForm + 9, 10, 5);
    ggl.gray.writeLine(xForm + 15, yForm, m_text, 10, 1, ggl.gray.BLACK);
    // если курсор над фреймом, то ждем нажатия на кнопку Ввода
    if ((_joy.posX0 >= xForm) && (_joy.posX0 <= xForm + 10) && ((_joy.posY0 >= yForm) && (_joy.posY0 <= yForm + 10)))
    {
        // u8g2.drawBox(xForm, yForm, 10, 10);
        ggl.gray.drawBox(xForm, yForm, 10, 10, ggl.gray.BLACK);

        if (_joy.pressKeyENTER() == true)
        {
            // если кнопка нажата
            // - если чекед 1 то меняем на 0
            // - если чекед 0 то меняем на 1
            switch (m_checked)
            {
            case true:
                m_checked = false;
                delay(250);
                break;
            case false:
                m_checked = true;
                delay(250);
                break;
            }
        }
    }
}
/* eFunction */
void eFunction::show()
{
    m_func();
}
/* ePicture */
void ePicture::show()
{
    // u8g2.drawXBMP(xForm, yForm, m_w, m_h, m_bitmap);
    ggl.gray.bitmap(xForm, yForm, m_bitmap, m_w, m_h, ggl.gray.NOT_TRANSPARENT);
}
/* eBackground */
void eBackground::show()
{
    // Проходим по всему окну с шагом, равным размеру изображения
    for (int y = yForm; y < 148; y += m_h)
    {
        for (int x = xForm; x < 256; x += m_w)
        {
            // Выводим изображение на текущей позиции (x, y)
            ggl.gray.bitmap(x, y, m_bitmap, m_w, m_h, ggl.gray.TRANSPARENT);
        }
    }
}
/* desktop */
template <typename T>
void eDesktop<T>::show()
{
    uint8_t border{4};
    uint8_t xx{border};
    uint8_t yy{16};

    uint8_t countTask{1};

    for (TaskArguments &t : data_)
    {
        if ((t.activ == false) && (t.bitMap != NULL) && (t.type == DESKTOP))
        {
            _shortcutDesktop.shortcut(t.name, t.bitMap, xx, yy, t.f, _joy.posX0, _joy.posY0);

            countTask++;

            xx += (32 + border);

            if (countTask > 7)
            {
                xx = 4;
                yy += (32 + border + 16);
                countTask = 0;
            }
        }
        else
        {
            // _gfx.print("There are no tasks to output\nto the desktop.", 5, 17);
        }
    }
}
/* exForm show */
int exForm::showForm()
{
    Button closeForm;

    int sizeStack = formsStack.size();

    switch (eFormShowMode)
    {
    case FULLSCREEN:
        // Для полноэкранного режима устанавливаем координаты относительно экрана
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x, element->m_y + 12, element->m_w, element->m_h);
        }
        break;
    case MAXIMIZED:
        // Для максимизированного режима устанавливаем координаты относительно окна
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x, element->m_y + 12, element->m_w, element->m_h);
        }
        break;
    case NORMAL:
        // Для нормального режима оставляем координаты без изменений
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x + 20, element->m_y + 26, element->m_w - 40, element->m_h);
        }
        break;
    }

    if (eFormShowMode == FULLSCREEN)
    {
        if (closeForm.button2("CLOSE", 225, 0, _joy.posX0, _joy.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        // u8g2.setColorIndex(1); // вкл пиксели
        // u8g2.drawFrame(0, 12 /* height button (13px) - 1 */, 256, 148); // x, y, w, h

        switch (eFormBackground)
        {
            case TRANSPARENT:
            ggl.gray.drawFrame(0, 12, 256, 148, ggl.gray.BLACK);
            break;
            case WHITE:
            ggl.gray.drawFillFrame(0, 12, 256, 148, ggl.gray.BLACK, ggl.gray.WHITE);
            break;
            case LIGHT_GRAY:
            ggl.gray.drawFillFrame(0, 12, 256, 148, ggl.gray.BLACK, ggl.gray.LIGHT_GRAY);
            break;
            case DARK_GRAY:
            ggl.gray.drawFillFrame(0, 12, 256, 148, ggl.gray.BLACK, ggl.gray.DARK_GRAY);
            break;
            case BLACK:
            ggl.gray.drawFillFrame(0, 12, 256, 148, ggl.gray.BLACK, ggl.gray.BLACK);
            break;
            default:
            ggl.gray.drawFrame(0, 12, 256, 148, ggl.gray.BLACK);
            break;
        }

        // ggl.gray.drawFrame(0, 12, 256, 148, ggl.gray.BLACK);
        _gfx.print(title, 5, 2, 10, 5); // size Font, text, x, y, lii, chi
        
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 205;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 200;
        }
        
        _gfx.print("[" + (String)sizeStack + "]", xSizeStack, 2, 10, 5);
    }

    if (eFormShowMode == MAXIMIZED)
    {
        if (closeForm.button2("CLOSE", 225, 0, _joy.posX0, _joy.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        // u8g2.setColorIndex(1); // вкл пиксели
        // u8g2.drawFrame(0, 12 /* height button - 1 */, 256, 137 /* 137 - 10px tray*/); // x, y, w, h
        
        switch (eFormBackground)
        {
            case TRANSPARENT:
            ggl.gray.drawFrame(0, 12, 256, 137, ggl.gray.BLACK);
            break;
            case WHITE:
            ggl.gray.drawFillFrame(0, 12, 256, 137, ggl.gray.BLACK, ggl.gray.WHITE);
            break;
            case LIGHT_GRAY:
            ggl.gray.drawFillFrame(0, 12, 256, 137, ggl.gray.BLACK, ggl.gray.LIGHT_GRAY);
            break;
            case DARK_GRAY:
            ggl.gray.drawFillFrame(0, 12, 256, 137, ggl.gray.BLACK, ggl.gray.DARK_GRAY);
            break;
            case BLACK:
            ggl.gray.drawFillFrame(0, 12, 256, 137, ggl.gray.BLACK, ggl.gray.BLACK);
            break;
            default:
            ggl.gray.drawFrame(0, 12, 256, 137, ggl.gray.BLACK);
            break;
        }
        
        // ggl.gray.drawFrame(0, 12, 256, 137, ggl.gray.BLACK);
        _gfx.print(title, 5, 2, 10, 5); // size Font, text, x, y, lii, chi
       
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 205;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 200;
        }
       
       _gfx.print("[" + (String)sizeStack + "]", xSizeStack, 2, 10, 5);
    }

    if (eFormShowMode == NORMAL)
    {

        if (closeForm.button2("CLOSE", 205, outerBoundaryForm - 12 + 6, _joy.posX0, _joy.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        // u8g2.setColorIndex(1); // вкл пиксели
        // u8g2.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120); // x, y, w, h

        switch (eFormBackground)
        {
            case TRANSPARENT:
            ggl.gray.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK);
            break;
            case WHITE:
            ggl.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK, ggl.gray.WHITE);
            break;
            case LIGHT_GRAY:
            ggl.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK, ggl.gray.LIGHT_GRAY);
            break;
            case DARK_GRAY:
            ggl.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK, ggl.gray.DARK_GRAY);
            break;
            case BLACK:
            ggl.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK, ggl.gray.BLACK);
            break;
            default:
            ggl.gray.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK);
            break;
        }
        
        // ggl.gray.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK);
        _gfx.print(title, outerBoundaryForm + 5, outerBoundaryForm - 4, 10, 5);
        
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 185;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 180;
        }

        _gfx.print("[" + (String)sizeStack + "]", xSizeStack, outerBoundaryForm - 4, 10, 5);
    }

    // выводим все элементы на дисплей
    for (auto element : elements)
    {
        element->show();
    }

    return 0; // 0 - the form works
}


/* Systems Element */
/* eBacklight */
void eBacklight::execute()
{
    //p-n-p transistor
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);

    if (m_stateLight == true)
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 0); // on
    }
    else
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 1); // off
    }
}
/* eDisplayContrast */
void eDisplayContrast::execute()
{
    // u8g2.setContrast(m_valueContrast);!!!
    // Serial.println(m_valueContrast);
    ggl.gray.setContrast(m_valueContrast);
}
/* ePowerSave */
bool ePowerSave::isTouched()
{
    if ((_joy.calculateIndexY0() == 0) && (_joy.calculateIndexX0() == 0)) return false;

    return true;
}
void ePowerSave::execute()
{
    String text = "The console has entered\nhibernation mode.\n\nTo exit this mode,\nmove the stick upwards.";
    
    ggl.gray.writeLine(17, 150, "Deep sleep LINE, leave cursor here", 10, 1, ggl.gray.LIGHT_GRAY);
    
    if (isTouched() == true)
    {
        screenTiming = TIMER;
        // u8g2.setPowerSave(0);  //off powersave!!!
        ggl.gray.setPowerMode(ggl.gray.OPERATING_MODE);
    }
    
    if ((TIMER - screenTiming > 60000) && (_joy.posY0 >= 150))
    {
        screenTiming = TIMER;

        while (isTouched() == false)
        {
            InstantMessage message1("The console has entered\ndeep sleep mode.", 3000);
            message1.show();

            systems.setBacklight(false);
            // u8g2.setPowerSave(1);         //off display!!!
            ggl.gray.setPowerMode(ggl.gray.SLEEP_MODE);
            esp_deep_sleep_start();       //run powersave, DEEP
        }
    }
    
    if ((TIMER - screenTiming > 60000) && (_joy.posY0 < 150))
    {
        screenTiming = TIMER;

        while (isTouched() == false)
        {
            InstantMessage message2(text, 3000);
            message2.show();

            systems.setBacklight(false);
            // u8g2.setPowerSave(0);          //on display!!!
            ggl.gray.setPowerMode(ggl.gray.OPERATING_MODE);
            esp_light_sleep_start();       //run powersave, LIGHT
        }
    }
}
/* eCursor */
void eCursor::execute()
{

}
/* eDataPort */
void eDataPort::execute()
{

}


/* Instant message */
/* Draw show function */
void InstantMessage::show()
{
    short border{5}, border2{8}, charHeight{10}, charWidth{5};
    int count{0}, x{128}, y{80}, maxChar{0}, line{1};

    for (char c : m_text) // Считаем символы в каждой строке
    {
        if (c == '\n')
        {
            if (count > maxChar)
                maxChar = count;
            count = 0;
            line++;
        }
        else
        {
            count++;
        }
    }

    // Проверяем последнюю строку, если она не заканчивается на '\n'
    if (count > maxChar)
    {
        maxChar = count;
    }

    if (maxChar == 0 || line == 0) {
        return; // Не отображаем ничего, если нет символов или строк
    }

    int lineYoffset = (line / 2) * 8;
      
    //--> выводим границы текста
    int numberOfPixels = maxChar * charWidth;               // Максимальное количество пикселей на основе количества символов в строке
    int numberOfPixelsToOffset = (maxChar / 2) * charWidth; // Смещение для центрирования текста

    //--> границы фрейма
    int frameX = x - numberOfPixelsToOffset - border2 + GLOBAL_X;
    int frameY = y - charHeight - border2 + GLOBAL_Y - lineYoffset;
    int frameWidth = border2 + border2 + numberOfPixels;
    int frameHeight = border2 + border2 + (line * charHeight);
    //<-- границы фрема

    // Рисуем заполненную рамку
    ggl.gray.drawFillFrame(x - numberOfPixelsToOffset - border2,
        y - border2 - lineYoffset, border2 + border2 + numberOfPixels, 
        border2 + border2 + (line * charHeight), 
        ggl.gray.WHITE, 
        ggl.gray.WHITE
    );
    
    // Выводим текст
    _gfx.print(m_text, x - numberOfPixelsToOffset, y - lineYoffset, charHeight, charWidth);

    // Рисуем внешнюю рамку
    ggl.gray.drawFrame(x - numberOfPixelsToOffset - border, 
        y - border - lineYoffset, 
        border + border + numberOfPixels, 
        border + border + (line * charHeight), 
        ggl.gray.BLACK
    );

    // Тень
    ggl.gray.drawHLine(
        x - numberOfPixelsToOffset - border + 3, // X: начало рамки
        y - lineYoffset + border + (line * charHeight), // Y: нижняя граница рамки
        border + numberOfPixels + border, // Длина: ширина рамки
        ggl.gray.DARK_GRAY, 3
    );

    ggl.gray.drawVLine(
        x - numberOfPixelsToOffset - border + (border + border + numberOfPixels), // X: начало рамки + ширина рамки + смещение
        y - border - lineYoffset + 3, // Y: начало рамки
        (line * charHeight) + border + border, // Высота: высота рамки
        ggl.gray.DARK_GRAY, 3
    );

    ggl.gray.sendBuffer();  // <--

    if (m_delay > 0) {
        delay(m_delay);
    }
}






//====================================================

//1
void _graphicsTest1(int xG, int yG, int wG, int hG)
{   
    for (int x = 0; x < 256; x += 2)
    {
        int y = random(12, 160);
        int h = random(160 - y);
        // u8g2.drawVLine(x, y, h);
        ggl.gray.drawVLine(x, y, h, ggl.gray.BLACK, 1); 
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

//2
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
                ggl.gray.drawBox(x, y, w, h, ggl.gray.BLACK);
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

//3
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
        ggl.gray.drawPixel(points[i][0], points[i][1], ggl.gray.BLACK);
    }

    // draw lines
    for (int i = 0; i < numPoints; i++)
    {
        for (int j = i + 1; j < numPoints; j++)
        {
            ggl.gray.drawLine(points[i][0], points[i][1], points[j][0], points[j][1], ggl.gray.BLACK);
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


//====================================================
/* Desktop */
void _myDesktop()
{
    exForm *form0 = new exForm();

    eDesktop<TaskArguments> *desktop0 = new eDesktop<TaskArguments>(tasks);
    ePicture *pic = new ePicture(sozos, 25, 100, 207, 38);
    // eBackground *bg0 = new eBackground(icon.pattern_1, 0, 0, 32, 32);
    
    form0->title = "Desktop";
    form0->eFormShowMode = FULLSCREEN;
    form0->addElement(pic);
    // form0->addElement(bg0);
    form0->addElement(desktop0);

    formsStack.push(form0);
    td.removeTaskIndex(100);
}
/* User Desktop */
void _userDesktop()
{
    exForm *form1 = new exForm();
    eDesktop<TaskArguments> *desktop1 = new eDesktop<TaskArguments>(userTasks);

    form1->title = "User Desktop";
    form1->eFormShowMode = FULLSCREEN;
    form1->addElement(desktop1);

    formsStack.push(form1);
}
/* Info OS */
void _info()
{
    exForm *formInfoSystems = new exForm();

    String text1 = "Founder of the platform and\nchief developer: Aleksander SAVUSHKIN\n\n";
    String text2 = "Github: @avsavushkiny, GitVerse: @avsavushkin\n\n";
    String text3 = "Developers: Sergey KSENOFONTOV, Michail SAMOYLOV,Aleksander MICHEEV, Ekaterina SYATKINA\n\n";
    String text4 = "Phone: +7 (953) 034 4001\nE-mail: aeondc@gmail.com\n\nSozvezdiye platform\nRussia, Saransk, 2023-2025";

    eTextBox *textBoxInfo = new eTextBox(text1 + text2 + text3 + text4, BorderStyle::noBorder, 256, 150, 0, 0);
    ePicture *pic1 = new ePicture(giga, 160, 100, giga_w, giga_h);

    formInfoSystems->title = "Information";
    formInfoSystems->eFormShowMode = FULLSCREEN;
    formInfoSystems->addElement(textBoxInfo);
    formInfoSystems->addElement(pic1);

    formsStack.push(formInfoSystems);
}
/* OS startup */
void _myOSstartupForm()
{
    exForm *formMyOSstartup = new exForm;
    eText *textMessage = new eText("I don't understand why you did this,\nbut oh well.\n\nTo launch Desktop - click on\nthe button below, good luck :))", 5, 5);
    eLine *line = new eLine(0, 97);
    eButton *button = new eButton("Run Desktop", _myDesktop, 5, 102);
    eButton *buttonReboot = new eButton("Reboot", _rebootBoard, 71, 102);
    eButton *buttonInfo = new eButton("Info", _info, 112, 102);

    formMyOSstartup->title = "OS startup";
    formMyOSstartup->eFormShowMode = NORMAL;

    formMyOSstartup->addElement(line);
    formMyOSstartup->addElement(textMessage);
    formMyOSstartup->addElement(button);
    formMyOSstartup->addElement(buttonReboot);
    formMyOSstartup->addElement(buttonInfo);

    formsStack.push(formMyOSstartup);
}
/* OS hello */
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
    td.removeTaskIndex(101);
}


/* Task. Stack, task, command */ 
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

void form4()
{
    exForm *form4 = new exForm();

    form4->title = "Robotics";
    form4->eFormShowMode = FULLSCREEN;

    formsStack.push(form4);
}


void _myDispatcherFunction(int xG, int yG, int wG, int hG)
{
    int xx{5}, yy{23};

    // u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    // u8g2.setColorIndex(1);// 0-off px, 1-on px

    // _gfx.print("Active tasks:", xx + 5, yy + 10, 8, 5);

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
            // labelTask.label(_ta.name, _ta.f, xx, yy);!!!
            // _gfx.print(_ta.name, xx, yy, 8, 5);
            
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
void _settingsForm()
{
    exForm *settingsForm = new exForm();
    uint8_t x{0}, y{0};

    // --> LED control
    eCheckbox *checkLED = new eCheckbox(systems.STATEBACKLIGHT, "backlight control", x + 5, y + 5);

    eFunction *funcLED = new eFunction([checkLED]()
                                       {
        systems.STATEBACKLIGHT = checkLED->isChecked();
        systems.setBacklight(systems.STATEBACKLIGHT); });

    // --> Set contrast
    eLabel *label1 = new eLabel((String)systems.VALUECONTRAST, x + 20, y + 23);

    eButton *button1 = new eButton("-", []()
                                   { systems.VALUECONTRAST -= 1; delay(250); systems.setDisplayContrast(systems.VALUECONTRAST); }, x + 5, y + 20);
    eButton *button2 = new eButton("+", []()
                                   { systems.VALUECONTRAST += 1; delay(250); systems.setDisplayContrast(systems.VALUECONTRAST); }, x+ 50, y + 20);

    eFunction *func1 = new eFunction([label1]()
                                     { label1->setText((String)systems.VALUECONTRAST); });

    eLabel *label2 = new eLabel("display contrast", x + 61, y + 23);
   
    // --> Title, show mode Form
    settingsForm->title = "Settings";
    settingsForm->eFormShowMode = NORMAL;

    // --> Add elements Form
    settingsForm->addElement(checkLED);
    settingsForm->addElement(funcLED);
    settingsForm->addElement(button1);
    settingsForm->addElement(button2);
    settingsForm->addElement(label1);
    settingsForm->addElement(func1);
    settingsForm->addElement(label2);
    // settingsForm->addElement(func2);
    // settingsForm->addElement(label3);

    // --> Push Form object
    formsStack.push(settingsForm);
}







/* Task. Reboot ESP32 */
void _rebootBoard()
{
    ESP.restart();
}

/* Cursor */
void _systemCursor()
{
    _joy.updatePositionXY(20);
    _crs.cursor(true, _joy.posX0, _joy.posY0);

    if ((_joy.pressKeyEX() == true) && (_joy.pressKeyENTER() == true))
    {
        // u8g2.setCursor(_joy.posX0 + 10, _joy.posY0 + 10); !!!
        // u8g2.print(_joy.posX0);
        // u8g2.setCursor(_joy.posX0 + 10, _joy.posY0 + 20);
        // u8g2.print(_joy.posY0);
    }
}

/*
    Task-dispatcher
    Dispatcher tasks, vector
    [01/2025, Alexander Savushkin]
*/
TaskArguments system0[] //0 systems, 1 desktop, 2 user
{
    {"desktop", _myDesktop, NULL, SYSTEM, 100, true},
    {"oshello", _osHello, NULL, SYSTEM, 101, true},
    {"form1", _myForm1, icon.window_abc, DESKTOP, 0, false},
    {"form2", _myForm2, icon.window_shell_1, DESKTOP, 0, false},
    {"form3", _myForm3, icon.window_shell_2, DESKTOP, 0, false},
    {"graphics 1", _myGraphicsTest1, icon.window_graphics, DESKTOP, 0, false},
    {"graphics 2", _myGraphicsTest2, icon.window_graphics, DESKTOP, 0, false},
    {"dispatcher", _myDispatcher, icon.app_wizard, DESKTOP, 0, false},
    {"graphics 3", _myGraphicsTest3, icon.window_graphics, DESKTOP, 0, false},
    {"settings", _settingsForm, icon.technical_group, DESKTOP, 0, false},
    {"userdesktop", _userDesktop, icon.program_manager, DESKTOP, 0, false},
    // [!] Last task
    {"cursor", _systemCursor, NULL, SYSTEM, 0, true}
};

int TaskDispatcher::sizeTasks()
{
    int sizeVector = tasks.size();
    return sizeVector;
}

void TaskDispatcher::addTask(const TaskArguments &task)
{
    tasks.push_back(task);
}

bool TaskDispatcher::removeTaskVector(const String &taskName)
{
    auto it = std::find_if(tasks.begin(), tasks.end(), [&taskName](const TaskArguments &task) { return task.name == taskName; });
    
    if (it != tasks.end())
    {
        tasks.erase(it);
        return true;
    }

    return false; 
}

bool TaskDispatcher::removeTask(const String &taskName)
{
    for (TaskArguments &t : tasks)
    {
        if ((t.activ) && (t.name == taskName))
        {
            t.activ = false; return true;
        }
    }
    return false;
}

bool TaskDispatcher::removeTaskIndex(const int index)
{
    for (TaskArguments &t : tasks)
    {
        if ((t.activ) && (t.index == index))
        {
            t.activ = false; return true;
        }
    }
    return false;
}

bool TaskDispatcher::runTask(const String &taskName)
{
    for (TaskArguments &t : tasks)
    {
        if ((!t.activ) && (t.name == taskName))
        {
            t.activ == true; return true;
        }
    }
    return false;
}


void runExFormStack()
{
    if (!formsStack.empty())
    {
        exForm *currentForm = formsStack.top();

        int result = currentForm->showForm();

        /*
        The form's "Close" button returns 1
        */

        if (result == 1)
        {
            formsStack.pop();
            delete currentForm;

            delay(250);
        }
    }
    if (formsStack.empty()) _myOSstartupForm();
}

void runTasksCore()
{
    for (TaskArguments &t : tasks)
    {
        if (t.activ)
        {
            runExFormStack();
            t.f();
        }
        // runExFormStack();
    }

    systems.executeAllSystemElements();
}

void TaskDispatcher::terminal3()
{
  TIMER = millis();
  
  _gfx.render(runTasksCore);
}

void addTasksForSystems()
{
    for (TaskArguments &t : system0)
    {
        tasks.push_back(t);
    }
}