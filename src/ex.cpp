/*
 [v 0.0.1] -> First release.
           - The first implementation. The terminal is used, the organization of tasks.
           - Create classes Graphics, Timer, Interface, Powersave, Melody, Button, Shortcut, Application, Label, Task.
           - Add Battery control, LED control tasks.
 [v 0.0.2] -> Organization, definition of events.
           - Add tasks: Reboot board, FPS.
           - Add powersave mode display
           - Fix bug-code
 [v 0.0.3] -> Using a vector for a task list.
           - Fix bug-code
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

#include <vector>
#include <functional>
#include <algorithm>

//version Library and Text
const int8_t VERSION_LIB[] = {0, 1, 5};

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

/* Graphics chip setup */
U8G2_ST75256_JLX256160_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/5, /* dc=*/17, /* reset=*/16);

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
/* backlight */
// bool Graphics::controlBacklight(bool state) //p-n-p transistor
// {
//     pinMode(PIN_BACKLIGHT_LCD, OUTPUT);

//     if (state == true)
//     {
//         digitalWrite(PIN_BACKLIGHT_LCD, 0); // on
//         return true;
//     }
//     else
//     {
//         digitalWrite(PIN_BACKLIGHT_LCD, 1); // off
//         return false;
//     }
// }
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
    u8g2.begin(); Serial.begin(9600);

    /* setting the resolution of the analog-to-digital converter */
    analogReadResolution(RESOLUTION_ADC);

    /* determine the backlight-port mode */
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);
    // you can immediately turn on the display backlight _gfx.controlBacklight(true);
    
    /* determine the operating modes of digital ports */
    pinMode(PIN_BUTTON_ENTER, INPUT);
    pinMode(PIN_BUTTON_EX,    INPUT);
    pinMode(PIN_BUTTON_A,     INPUT);
    pinMode(PIN_BUTTON_B,     INPUT);
    pinMode(PIN_BATTERY,      INPUT);

    /* turn off display backlight */
    // _gfx.controlBacklight(false);
    systems.setBacklight(false);
    /* setting display, contrast */
    // u8g2.setContrast(143); //143//150
    // systems.setDisplayContrast(143);
    systems.executeAllSystemElements();


    /*
       Vector
       moving system-task to the vector
       determine the number of tasks in the vector
    */
    addTasksForSystems();
    //_taskSystems.reserve(50); // reserve
    //int _ssize = td.sizeTasks(tasks);

    /*
        UI-begin
        display detailed information
    */
    u8g2.clearBuffer();
    // u8g2.drawXBMP(((W_LCD - image_width)/2), ((H_LCD - image_height)/2) - 7, image_width, image_height, ex_bits);
    _textBox.textBox("Sozvezdiye OS\n\nExperiment board\n2024", _textBox.middle, _textBox.noBorder, 10, 6, 128, 80);

    _gfx.print(6, (String)VERSION_LIB[0] + "." + (String)VERSION_LIB[1] + "." + (String)VERSION_LIB[2], 0, H_LCD, 10, 4);
    //_gfx.print(6, (String)_ssize, 0, 6, 10, 6);
    u8g2.sendBuffer();

    delay(1500);
}
/* data render (full frame) */
void Graphics::render(void (*f)(), int timeDelay)
{
    uint32_t time;

    time = millis() + timeDelay;

    do
    {
        u8g2.clearBuffer();
        f();
        u8g2.sendBuffer();
    } while (millis() < time);
}
/* data render (full frame) no time delay */
void Graphics::render(void (*f)())
{
    u8g2.clearBuffer();
    f();
    u8g2.sendBuffer();
}
/* data render two function (full frame) */
void Graphics::render(void (*f1)(), void (*f2)())
{
      u8g2.clearBuffer();
      f1();
      f2();
      u8g2.sendBuffer();
}
/* clearing the output buffer */
void Graphics::clear()
{
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}
/* text output with parameters, add size font, add line interval (def: 10) and character interval (def: 6) */
void Graphics::print(int8_t sizeFont, String text, int x, int y, int8_t lii, int8_t chi) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int sizeText = text.length() + 1;
    int yy{0};

    //micro, 6, 7, 8, 10, 12, 13
    if (sizeFont == 5) u8g2.setFont(u8g2_font_micro_tr);
    else if (sizeFont == 6) u8g2.setFont(u8g2_font_4x6_tr);
    else if (sizeFont == 7) u8g2.setFont(u8g2_font_5x7_tr);
    else if (sizeFont == 8) u8g2.setFont(u8g2_font_5x8_tr);
    else if (sizeFont == 10) u8g2.setFont(u8g2_font_6x10_tr);
    else if (sizeFont == 12) u8g2.setFont(u8g2_font_6x12_tr);
    else if (sizeFont == 13) u8g2.setFont(u8g2_font_6x13_tr);
    else u8g2.setFont(u8g2_font_6x10_tr); //default

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}
/* text output with parameters, add line interval (def: 10) and character interval (def: 6) */
void Graphics::print(String text, int x, int y, int8_t lii, int8_t chi) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int sizeText = text.length() + 1;
    int yy{0};

    u8g2.setFont(u8g2_font_6x10_tr);

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}
/* text output with parameters */
void Graphics::print(String text, int x, int y) // text, x-position, y-position, line interval (8-10), character interval (4-6)
{
    int8_t lii{10}, chi{6};
    int sizeText = text.length() + 1;
    int yy{0};

    u8g2.setFont(u8g2_font_6x10_tr);

    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }
}
/* "wink text" output  */
bool Graphics::winkPrint(void (*f)(String, int, int), String text, int x, int y, int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime_0 >= interval)
    {
        prevTime_0 = currTime;
        return 0;
    }
    else
    {
        f(text, x, y);
        return 1;
    }
}
/* wait display */
bool Graphics::waitDisplay()
{
    u8g2.clearBuffer();
    u8g2.drawXBMP(((W_LCD - wait_width)/2), ((H_LCD - wait_height)/2), wait_width, wait_height, wait_bits);
    delay(150);
    u8g2.sendBuffer(); return true;
}
/* Calculate FPS */
int Graphics::calculateFPS()
{
    // get FPS
    if ((millis() - _fpsTime) <= 1000)
    {
        _fpsCounter++;
    }
    else
    {
        _fpsTime = millis();
        _FPS = _fpsCounter;
        _fpsCounter = 0;
    }
    return _FPS;
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
        u8g2.setDrawColor(1);  //0-white 1-black 2-inversion
        u8g2.setBitmapMode(1); //0-non transparent 1-transparent

        u8g2.drawXBMP(xCursor, yCursor, cursor_w, cursor_h, cursor_bits);
        
        u8g2.setDrawColor(1);
        u8g2.setBitmapMode(0);
        u8g2.setColorIndex(1);
        return true;
    }
    else
        return false;
}
/* displaying the cursor on the screen */
bool Cursor::cursor(bool stateCursor)
{
    if (stateCursor == true)
    {
        u8g2.setDrawColor(1);  //0-white 1-black 2-inversion
        u8g2.setBitmapMode(1); //0-non transparent 1-transparent
        _joy.updatePositionXY();
        u8g2.drawXBMP(_joy.posX0, _joy.posY0, cursor_w, cursor_h, cursor_bits);
        u8g2.setDrawColor(1);
        u8g2.setBitmapMode(0);
        return true;
    }
    else
        return false;
}


/* 
    class
    Interface
*/
/* displaying a message to the user */
void Interface::message(String text, int duration)
{
    uint8_t x{10}, y{34};
    
    /* Counting the number of lines in a message
       Line break is supported - '\n' */   
    int sizeText = text.length() + 1, line{};

    for (int i = 0; i < sizeText; i++)
    {
        if (text[i] == '\n')
        {
          line++;
        }
    }

    uint8_t correction = (line * 10)/2;
    
    u8g2.clearBuffer();
    for (int i = 0; i < W_LCD; i += 2)
    {
        for (int j = 0; j < (y - 10) - correction; j += 2)
        {
            u8g2.drawPixel(i, j);
        }

        for (int k = y + ((line) * 10) + 4 - correction /* correction */; k < H_LCD; k += 2)
        {
            u8g2.drawPixel(i, k);
        }
    }

    _gfx.print(text, x, y - correction, 10, 6);
    u8g2.sendBuffer();

    delay(duration);
}
/* displaying a message to the user */
void Interface::popUpMessage(String label, String text, uint tDelay)
{
    uint8_t sizeText = text.length();

    uint8_t countLine{1}, countChar{0}, maxChar{}, h_frame{}, border{5}, a{}; 

    for (int i = 0; i <= sizeText; i++)
    {
        if (text[i] != '\0')
        {
            ++countChar;

            if (text[i] == '\n')
            {
                countLine++;

                if ((text[i] == '\n') && (countChar > maxChar))
                {
                    maxChar = countChar;
                    countChar = 0;
                }
            }

            if ((text[i] == '\0') || (text[i+1] == '\0'))
            {
                if (countChar > maxChar)
                {
                    maxChar = countChar;
                    countChar = 0;
                }
            }
            if (countChar > maxChar) maxChar = countChar;
        }
    }
    
    h_frame = countLine * 10; a = h_frame/2;

    u8g2.clearBuffer(); // -->
    u8g2.drawFrame(((W_LCD/2)-(maxChar*6)/2) - border, (H_LCD/2) - a - border, (maxChar * 6) + (border * 2), h_frame + (border * 2));
    u8g2.drawFrame(((W_LCD/2)-(maxChar*6)/2) - (border + 3), (H_LCD/2) - a - (border + 3), (maxChar * 6) + ((border + 3) * 2), h_frame + ((border + 3) * 2));

    _gfx.print(label, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a - (border + 4));
    _gfx.print(text, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a + 10);
    u8g2.sendBuffer();  // <--

    delay(tDelay);

    //debug
    /*Serial.println(maxChar); Serial.println(sizeText);
    for (int i = 0; i <= sizeText; i++)
    {
        Serial.println(text[i], BIN);
    }*/
}
/* displaying a message to the user */
void Interface::popUpMessage(String label1, String label2, String text, uint tDelay)
{
    uint8_t sizeText = text.length();

    uint8_t countLine{1}, countChar{0}, maxChar{}, h_frame{}, border{5}, a{}; 

    for (int i = 0; i <= sizeText; i++)
    {
        if (text[i] != '\0')
        {
            ++countChar;

            if (text[i] == '\n')
            {
                countLine++;

                if ((text[i] == '\n') && (countChar > maxChar))
                {
                    maxChar = countChar;
                    countChar = 0;
                }
            }

            if ((text[i] == '\0') || (text[i+1] == '\0'))
            {
                if (countChar > maxChar)
                {
                    maxChar = countChar;
                    countChar = 0;
                }
            }
            if (countChar > maxChar) maxChar = countChar;
        }
    }
    
    h_frame = countLine * 10; a = h_frame/2;

    u8g2.clearBuffer();
    u8g2.drawFrame(((W_LCD/2)-(maxChar*6)/2) - border, (H_LCD/2) - a - border, (maxChar * 6) + (border * 2), h_frame + (border * 2));
    u8g2.drawFrame(((W_LCD/2)-(maxChar*6)/2) - (border + 3), (H_LCD/2) - a - (border + 3), (maxChar * 6) + ((border + 3) * 2), h_frame + ((border + 3) * 2));

    _gfx.print(label1, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a - (border + 4));
    _gfx.print(text, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a + 10);
    _gfx.print(label2, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) + a + (border + 11));

    u8g2.sendBuffer();

    delay(tDelay);

    //debug
    /*Serial.println(maxChar); Serial.println(sizeText);
    for (int i = 0; i <= sizeText; i++)
    {
        Serial.println(text[i], BIN);
    }*/
}
/* displaying a message to the user */
void Interface::popUpMessage(String label, String text)
{
    uint8_t sizeText = text.length();

    uint8_t countLine{1}, countChar{0}, maxChar{}, h_frame{}, border{5}, a{}; 

    for (int i = 0; i <= sizeText; i++)
    {
        if (text[i] != '\0')
        {
            ++countChar;

            if (text[i] == '\n')
            {
                countLine++;

                if ((text[i] == '\n') && (countChar > maxChar))
                {
                    maxChar = countChar;
                    countChar = 0;
                }
            }

            if ((text[i] == '\0') || (text[i+1] == '\0'))
            {
                if (countChar > maxChar)
                {
                    maxChar = countChar;
                    countChar = 0;
                }
            }
            if (countChar > maxChar) maxChar = countChar;
        }
    }
    
    h_frame = countLine * 10; a = h_frame/2;

    xBorder = 128 /* W_LCD/2 */ - (maxChar*6)/2;
    yBorder = 80  /* H_LCD/2 */ + a + (border + 11);

    u8g2.clearBuffer();
    u8g2.drawFrame(((W_LCD/2)-(maxChar*6)/2) - border, (H_LCD/2) - a - border, (maxChar * 6) + (border * 2), h_frame + (border * 2));
    u8g2.drawFrame(((W_LCD/2)-(maxChar*6)/2) - (border + 3), (H_LCD/2) - a - (border + 3), (maxChar * 6) + ((border + 3) * 2), h_frame + ((border + 3) * 2));

    _gfx.print(label, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a - (border + 4));
    _gfx.print(text, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a + 10);
    u8g2.sendBuffer();

    //Serial.println(max); Serial.println(countChar);
}
/* displaying the dialog to the user */
bool Interface::dialogueMessage(String label, String text, void (*f1)(), void (*f2)()){}
/* displaying the dialog to the user */
bool Interface::dialogueMessage(String label, String text)
{
    while (true)
    {
        uint8_t sizeText = text.length();

        uint8_t countLine{1}, countChar{0}, maxChar{}, h_frame{}, border{5}, a{};        
        
        for (int i = 0; i <= sizeText; i++)
        {
            if (text[i] != '\0')
            {
                ++countChar;

                if (text[i] == '\n')
                {
                    countLine++;

                    if ((text[i] == '\n') && (countChar > maxChar))
                    {
                        maxChar = countChar;
                        countChar = 0;
                    }
                }

                if ((text[i] == '\0') || (text[i + 1] == '\0'))
                {
                    if (countChar > maxChar)
                    {
                        maxChar = countChar;
                        countChar = 0;
                    }
                }
                if (countChar > maxChar)
                    maxChar = countChar;
            }
        }

        h_frame = countLine * 10;
        a = h_frame / 2;

        xBorder = 128 /* W_LCD/2 */ - (maxChar * 6) / 2;
        yBorder = 80  /* H_LCD/2 */ + a + (border + 11);

        u8g2.clearBuffer();
        u8g2.drawFrame(((W_LCD / 2) - (maxChar * 6) / 2) - border, (H_LCD / 2) - a - border, (maxChar * 6) + (border * 2), h_frame + (border * 2));
        u8g2.drawFrame(((W_LCD / 2) - (maxChar * 6) / 2) - (border + 3), (H_LCD / 2) - a - (border + 3), (maxChar * 6) + ((border + 3) * 2), h_frame + ((border + 3) * 2));

        _gfx.print(label, (W_LCD / 2) - (maxChar * 6) / 2, (H_LCD / 2) - a - (border + 4));
        _gfx.print(text, (W_LCD / 2) - (maxChar * 6) / 2, (H_LCD / 2) - a + 10);

        //_ok.button(" OK ", xBorder - 8, yBorder + 2, _joy.posX0, _joy.posY0);
        //_no.button(" NO ", xBorder + 20, yBorder + 2, _joy.posX0, _joy.posY0);

        if (_ok.button(" OK ", 128 - 26, yBorder + 2, _joy.posX0, _joy.posY0))
        {
            return true;
            break;
        }
        if (_no.button(" NO ", 130, yBorder + 2, _joy.posX0, _joy.posY0))
        {
            return false;
            break;
        }

        _joy.updatePositionXY(25);
        _crs.cursor(true, _joy.posX0, _joy.posY0);
        u8g2.sendBuffer();
    }
}


/* 
    class
    Button
*/
/* button return boolean state */
bool Button::button(String text, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  uint8_t sizeText = text.length();

  if ((xCursor >= x && xCursor <= (x + (sizeText * 5) + 4)) && (yCursor >= y - 8 && yCursor <= y + 2))
  {
    u8g2.setDrawColor(1);
    u8g2.drawBox(x, y - 8, (sizeText * 5) + 5, 10);

    if (Joystick::pressKeyENTER() == true)
    {
      f();
      return true;
    }
  }
  else
  {
    u8g2.setDrawColor(1);
    u8g2.drawFrame(x, y - 8, (sizeText * 5) + 5, 10);
  }

  u8g2.setCursor(x + 3, y);
  u8g2.setFont(u8g2_font_profont10_mr);
  u8g2.setFontMode(1);
  u8g2.setDrawColor(2);
  u8g2.print(text);
  u8g2.setFontMode(0);
  
  return false;
}
/* button return boolean state */
bool Button::button(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor)
{
  uint8_t sizeText = text.length();

  if ((xCursor >= x && xCursor <= (x + (sizeText * 5) + 4)) && (yCursor >= y - 8 && yCursor <= y + 2))
  {
    //u8g2.setDrawColor(1);
    u8g2.drawBox(x, y - 8, (sizeText * 5) + 5, 10);

    if (Joystick::pressKeyENTER() == true)
    {
      return true;
    }
  }
  else
  {
    //u8g2.setDrawColor(1);
    u8g2.drawFrame(x, y - 8, (sizeText * 5) + 5, 10);
  }

  u8g2.setCursor(x + 3, y);
  u8g2.setFont(u8g2_font_profont10_mr);

  u8g2.setFontMode(1);

  u8g2.setDrawColor(2);
  u8g2.print(text);
  u8g2.setDrawColor(1);
  
  u8g2.setFontMode(1);
  
  return false;
}

bool Button::button2(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor)
{
  uint8_t sizeText = text.length(); short border{3}; short charW{5};

    u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    u8g2.setBitmapMode(0);// 0-off, 1-on Transporent mode
    u8g2.setColorIndex(1);// 0-off px, 1-on px

  if ((xCursor >= x && xCursor <= (x + (sizeText * charW) + border + border)) && (yCursor >= y && yCursor <= y + 13))
  {
    u8g2.setColorIndex(1); // включаем пиксели
    u8g2.drawBox(x, y, (sizeText * charW) + border + border, 13); // рисуем черный бокс
    u8g2.setColorIndex(0); // отключаем птксели
    _gfx.print(text, x + border, y + 7 /* font H */ + border, 8, charW); // выводим тест
    u8g2.setColorIndex(1); // включаем пиксели

    if (Joystick::pressKeyENTER() == true)
    {
      return true;
    }
  }
  else
  {
    u8g2.setColorIndex(1); // включаем пиксели
    u8g2.drawFrame(x, y, (sizeText * charW) + border + border, 13); // рисуем прозрачный фрейм
    _gfx.print(text, x + border, y + 7 /* font H */ + border, 8, charW); // выводим тест
  }
  
  return false;
}


/* 
    class
    Shortcut
*/
/* displaying a shortcut to a task-function */
bool Shortcut::shortcut(const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  u8g2.setDrawColor(1);
  u8g2.setBitmapMode(0);
  u8g2.drawXBMP(x, y, 32, 32, bitMap);
  //u8g2.drawXBMP(x, y + 24, 8, 8, icon_bits);
  u8g2.drawXBMP(x, y + 21, 11, 11, shortcut_bits);

  if ((xCursor >= x && xCursor <= (x + 32)) && (yCursor >= y && yCursor <= (y + 32)))
  {
    u8g2.drawFrame(x, y, 32, 32);
    if (Joystick::pressKeyENTER() == true)
    {
      f();
      return true;
    }
  }

  return false;
}
/* displaying a shortcut to a task-function */
bool Shortcut::shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  u8g2.setDrawColor(1);
  u8g2.setBitmapMode(0);
  u8g2.drawXBMP(x, y, 32, 32, bitMap);
  u8g2.drawXBMP(x, y + 21, 11, 11, shortcut_bits);
  
  TextBox textBoxNameTask;

  if ((xCursor >= x && xCursor <= (x + 32)) && (yCursor >= y && yCursor <= (y + 32)))
  {
    u8g2.drawFrame(x, y, 32, 32);

    textBoxNameTask.textBox(name, 16, 32, 8, 5, x, y + 32);
    
    if (Joystick::pressKeyENTER() == true)
    {
      _gfx.waitDisplay();
      f();
      return true;
    }
  }
  else
  {
    textBoxNameTask.textBox(name, 16, 32, 8, 5, x, y + 32);
  }

  return false;
}


/* displaying a shortcut to a task-function */
bool Shortcut::shortcutFrame(String name, uint8_t w, uint8_t h, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
    u8g2.setDrawColor(1);
    u8g2.setBitmapMode(0);

    //u8g2.drawXBMP(x, y + 24, 8, 8, icon_bits);

    if ((xCursor >= x && xCursor <= (x + w)) && (yCursor >= y && yCursor <= (y + h)))
    {
        u8g2.drawFrame(x, y, w, h);

        // BUFFER_STRING = name;

        if (Joystick::pressKeyENTER() == true)
        {
            f();
            return true;
        }
    }
    else
    {
    }

    return false;
}


/* 
    class
    Label
*/
/* text object as a link */
bool Label::label(String text, uint8_t x, uint8_t y, void (*f)(void), uint8_t lii, uint8_t chi, int xCursor, int yCursor)
{
    /*u8g2.setDrawColor(1);
    u8g2.setBitmapMode(0);

    uint8_t w{},h{}; 

    uint8_t sizeText = text.length();
    w = sizeText * chi;
    h = lii;

    _gfx.print(text, x, y, lii, chi);

    if ((xCursor >= x && xCursor <= (x + w)) && (yCursor >= y && yCursor <= (y + h)))
    {
        u8g2.drawFrame(x, y, w, h);

        BUFFER_STRING = text;

        if (Joystick::pressKeyENTER() == true)
        {
            f();
            return true;
        }
    }
    else
    {
    }*/

    uint8_t sizeText = text.length();
    uint8_t yy{};

    if ((xCursor >= x && xCursor <= (x + (sizeText * chi))) && (yCursor >= y - (lii + 2) && yCursor <= y + 2))
    {
        u8g2.setDrawColor(1);
        u8g2.drawBox(x - 1, y - (lii), (sizeText * chi) + 2, lii + 1);

        // BUFFER_STRING = text;

        if (Joystick::pressKeyENTER() == true)
        {
            f();
            return true;
        }
    }
    else
    {
        u8g2.setDrawColor(1);
    }

    u8g2.setCursor(x + 3, y);
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    
    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(text[i]);

        if (text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }

    u8g2.setFontMode(0);

    return false;
}
/* text object as a link, sends the description to the output buffer */
void Label::label(String text, void (*f)(void), uint8_t xLabel, uint8_t yLabel)
{
    uint8_t sizeText = text.length();
    uint8_t yy{}, chi{5}, lii{8}; int x{xLabel + 1}, y{yLabel + 8};

    if ((_joy.posX0 >= x && _joy.posX0 <= (x + (sizeText * chi))) && (_joy.posY0 >= y - (lii + 2) && _joy.posY0 <= y + 2))
    {

        u8g2.setColorIndex(1);                                         // включаем пиксели
        u8g2.drawBox(x - 1, y - (lii), (sizeText * chi) + 2, lii + 1); // рисуем черный бокс
        u8g2.setColorIndex(0);                                         // отключаем пиксели

        u8g2.setCursor(x + 3, y);
        u8g2.setFont(u8g2_font_6x10_tr);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            u8g2.setCursor(xx + x, yy + y);
            u8g2.print(text[i]);

            if (text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
        u8g2.setColorIndex(1);                                          // включаем пиксели


        if (_joy.pressKeyENTER() == true)
        {
            _gfx.waitDisplay();
            f(); 
        }
    }
    else
    {
        u8g2.setColorIndex(1);                                          // включаем пиксели

        u8g2.setCursor(x + 3, y);
        u8g2.setFont(u8g2_font_6x10_tr);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            u8g2.setCursor(xx + x, yy + y);
            u8g2.print(text[i]);

            if (text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
    }    
}

/* 
     class
     TextBox
*/
/* dynamic Frame */
void TextBox::textBox(String str, objectLocation location, objectBoundary boundary, short charH, short charW, int x, int y)
{
    short border{5}; short border2{8}; // size border
    short charHeight{charH}; short charWidth{charW}; // size char
    int count{0}; int maxChar{0}; // for counting characters
    int line{1}; // there will always be at least one line of text in the text

    for (char c : str) // count the characters in each line
    {
        if (c == '\n')
        {
            if (count > maxChar) maxChar = count;
            count = 0; line++;
        }
        else
        {
            count++;
        }
    }

    int numberOfPixels = maxChar * charWidth; // the maximum number of pixels based on the number of characters in a line
    int numberOfPixelsToOffset = (maxChar / 2) * charWidth; // a given number of pixels must be offset

    /* location */
    if (location == middle)
    {
        _gfx.print(str, x - numberOfPixelsToOffset + GLOBAL_X, y + GLOBAL_Y, charHeight, charWidth);

        if (boundary == noBorder){ /* we don't draw anything */ }
        if (boundary == oneLine)
        {
            u8g2.drawFrame(x - numberOfPixelsToOffset - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
        }
        if (boundary == twoLine)
        {
            u8g2.drawFrame(x - numberOfPixelsToOffset - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
            u8g2.drawFrame(x - numberOfPixelsToOffset - border2 + GLOBAL_X, y - charHeight - border2 + GLOBAL_Y, border2 + border2 + numberOfPixels, border2 + border2 + (line * charHeight));
        }
        if (boundary == shadow)
        {
            u8g2.drawFrame(x - numberOfPixelsToOffset - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
            // draw line 1
            u8g2.drawHLine(x - numberOfPixelsToOffset - border + 1 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixelsToOffset + border + GLOBAL_X, y - charHeight - border + 1 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
            // draw line 2
            u8g2.drawHLine(x - numberOfPixelsToOffset - border + 2 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + 1 /*px*/ + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixelsToOffset + border + 1 /*px*/ + GLOBAL_X, y - charHeight - border + 2 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
        }
        if (boundary == shadowNoFrame)
        {
            // draw line 1
            u8g2.drawHLine(x - numberOfPixelsToOffset - border + 1 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixelsToOffset + border + GLOBAL_X, y - charHeight - border + 1 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
            // draw line 2
            u8g2.drawHLine(x - numberOfPixelsToOffset - border + 2 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + 1 /*px*/ + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixelsToOffset + border + 1 /*px*/ + GLOBAL_X, y - charHeight - border + 2 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
        }
    }

    if (location == left)
    {
        _gfx.print(str, x + GLOBAL_X, y + GLOBAL_Y, charHeight, charWidth);
        
        if (boundary == noBorder){ /* we don't draw anything */ }
        if (boundary == oneLine)
        {
            u8g2.drawFrame(x - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
        }
        if (boundary == twoLine)
        {
            u8g2.drawFrame(x - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
            u8g2.drawFrame(x - border2 + GLOBAL_X, y - charHeight - border2 + GLOBAL_Y, border2 + border2 + numberOfPixels, border2 + border2 + (line * charHeight));
        }
        if (boundary == shadow)
        {
            u8g2.drawFrame(x - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
            // draw line 1
            u8g2.drawHLine(x - border + 1 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixels + border + GLOBAL_X, y - charHeight - border + 1 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
            // draw line 2
            u8g2.drawHLine(x - border + 2 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + 1 /*px*/ + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixels + border + 1 /*px*/ + GLOBAL_X, y - charHeight - border + 2 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
        }
        if (boundary == shadowNoFrame)
        {
            // draw line 1
            u8g2.drawHLine(x - border + 1 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixels + border + GLOBAL_X, y - charHeight - border + 1 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
            // draw line 2
            u8g2.drawHLine(x - border + 2 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + 1 /*px*/ + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + numberOfPixels + border + 1 /*px*/ + GLOBAL_X, y - charHeight - border + 2 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
        }
    }

    if (location == right)
    {
        _gfx.print(str, x - numberOfPixels + GLOBAL_X, y + GLOBAL_Y);
        
        if (boundary == noBorder){ /* we don't draw anything */ }
        if (boundary == oneLine)
        {
            u8g2.drawFrame(x - numberOfPixels - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
        }
        if (boundary == twoLine)
        {
            u8g2.drawFrame(x - numberOfPixels - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
            u8g2.drawFrame(x - numberOfPixels - border2 + GLOBAL_X, y - charHeight - border2 + GLOBAL_Y, border2 + border2 + numberOfPixels, border2 + border2 + (line * charHeight));
        }
        if (boundary == shadow)
        {
            u8g2.drawFrame(x - numberOfPixels - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y, border + border + numberOfPixels, border + border + (line * charHeight));
            // draw line 1
            u8g2.drawHLine(x - numberOfPixels - border + 1 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + border + GLOBAL_X, y - charHeight - border + 1 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
            // draw line 2
            u8g2.drawHLine(x - numberOfPixels - border + 2 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + 1 /*px*/ + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + border + 1 /*px*/ + GLOBAL_X, y - charHeight - border + 2 /*px*/ + GLOBAL_Y, border + border + (line * charHeight)); 
        }
        if (boundary == shadowNoFrame)
        {
            // draw line 1
            u8g2.drawHLine(x - numberOfPixels - border + 1 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + border + GLOBAL_X, y - charHeight - border + 1 /*px*/ + GLOBAL_Y, border + border + (line * charHeight));
            // draw line 2
            u8g2.drawHLine(x - numberOfPixels - border + 2 /*px*/ + GLOBAL_X, y + border + ((line - 1) * charHeight) + 1 /*px*/ + GLOBAL_Y, numberOfPixels + border + border);
            u8g2.drawVLine(x + border + 1 /*px*/ + GLOBAL_X, y - charHeight - border + 2 /*px*/ + GLOBAL_Y, border + border + (line * charHeight)); 
        }
    }

    //debug
    //_gfx.print((String)maxChar, 0, 30);
    //_gfx.print((String)line, 0, 40);
}
/* static Frame */
void TextBox::textBox(String str, objectBoundary boundary, int sizeH, int sizeW, short charH, short charW, int x, int y)
{
    short border{5}; short border2{8}; // size border
    int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
    int line{1}; // there will always be at least one line of text in the text
    int numberOfCharacters{0}; // количество символов
    int ch{0}, ln{0}; int xx{x}, yy{y}; 
    
    //String textObject = str; // object

    if (boundary == noBorder){ /* we don't draw anything */ }
    if (boundary == oneLine)
    {
        u8g2.drawFrame(x, y, sizeW, sizeH);
    }
    if (boundary == twoLine)
    {
        u8g2.drawFrame(x, y, sizeW, sizeH);
        u8g2.drawFrame(x - 3, y - 3, sizeW + 6, sizeH + 6);
    }
    if (boundary == shadow)
    {
        u8g2.drawFrame(x, y, sizeW, sizeH);

        u8g2.drawHLine(x + 1, y + sizeH, sizeW);
        u8g2.drawHLine(x + 2, y + sizeH + 1, sizeW);

        u8g2.drawVLine(x + sizeW, y + 1, sizeH);
        u8g2.drawVLine(x + sizeW + 1, y + 2, sizeH);
    }
    if (boundary == shadowNoFrame)
    {
        u8g2.drawHLine(x + 1, y + sizeH, sizeW);
        u8g2.drawHLine(x + 2, y + sizeH + 1, sizeW);

        u8g2.drawVLine(x + sizeW, y + 1, sizeH);
        u8g2.drawVLine(x + sizeW + 1, y + 2, sizeH);
    }
    
    
    for (char c : str)
    {
        numberOfCharacters++;
    }

    int numberOfCharactersLineFrame = (sizeW - border - border) / charW; // количество символов в строчке Фрейма
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
    int numberOfLinesFrame = (sizeH - border - border) / charH; // количество строчек в Фрейме
    
    for (char c : str)
    {
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(xx + border, yy + charH + border);
        u8g2.print(c);

        // adds dots if frame is full
        /*if ((ln >= numberOfLinesFrame - 1) && (numberOfCharactersLineFrame == (ch + 3)))
        {
            u8g2.print("...");
        }
        else u8g2.print(c);*/
        
        xx += charW;
        ch++;

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH; ch = 0; xx = x; ln++;
        }

        if (ln >= numberOfLinesFrame)
        {
            // draw glyph
            /*u8g2.setFont(u8g2_font_unifont_t_symbols);
            u8g2.drawGlyph(x + sizeW - 8, y + sizeH + 2, 0x2198);*/

            u8g2.drawTriangle(x + sizeW, y + sizeH - 8, x + sizeW, y + sizeH, x + sizeW - 8, y + sizeH);
            break;
        }
    }
    //debug
    //_gfx.print((String)numberOfLines, 0, 30);
    //_gfx.print((String)numberOfLinesFrame, 0, 40);
}
/* no Frame */
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
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(xx + border, yy + charH + border);
        u8g2.print(c);
        
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
/* ---> remove support */
void Timer::stopwatch(void (*f)(void), int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        f();
    }
}


/* 
    class
    Melody
*/
/* playing a melody */
void songEngine(uint arr[][2], uint noteCount)
{
  for (uint i = 0; i < noteCount; i++)
  {
    tone(PIN_BUZZER, arr[i][0], arr[i][1]);
    delay(120);
    noTone(PIN_BUZZER);
  }
}
/* list melody */
void Melody::songCore()
{
    switch (lM)
    {
    case listMelody::None:
        break;

    // Melody
    case listMelody::MakeGame:
        songEngine(songMakeGame, 7);
        lM = None;
        break;
    
    // Bang
    case listMelody::Bang1:
        songEngine(songBang1, 2);
        lM = None;
        break;

    case listMelody::Bang2:
        songEngine(songBang2, 2);
        lM = None;
        break;

    case listMelody::Bang3:
        songEngine(songBang3, 2);
        lM = None;
        break;

    case listMelody::Bang4:
        songEngine(songBang4, 2);
        lM = None;
        break;

    case listMelody::Bang5:
        songEngine(songBang5, 2);
        lM = None;
        break;

    // Tone
    case listMelody::Tone1:
        songEngine(songTone1, 1);
        lM = None;
        break;

    case listMelody::Tone2:
        songEngine(songTone2, 1);
        lM = None;
        break;

    case listMelody::Tone3:
        songEngine(songTone3, 1);
        lM = None;
        break;

    case listMelody::Tone4:
        songEngine(songTone4, 1);
        lM = None;
        break;

    case listMelody::Tone5:
        songEngine(songTone5, 1);
        lM = None;
        break;

    // UI
    // --

    default:
        break;
    }
}
/* list melody */
void Melody::song(listMelody num)
{
    switch (num)
    {
    case listMelody::None:
        lM = None;
        break;
    
    // Melody
    case listMelody::MakeGame:
        lM = MakeGame;
        break;

    // Bang
    case listMelody::Bang1:
        lM = Bang1;
        break;

    case listMelody::Bang2:
        lM = Bang2;
        break;

    case listMelody::Bang3:
        lM = Bang3;
        break;

    case listMelody::Bang4:
        lM = Bang4;
        break;

    case listMelody::Bang5:
        lM = Bang5;
        break;
    
    // Tones
    case listMelody::Tone1:
        lM = Tone1;
        break;

    case listMelody::Tone2:
        lM = Tone2;
        break;

    case listMelody::Tone3:
        lM = Tone3;
        break;

    case listMelody::Tone4:
        lM = Tone4;
        break;

    case listMelody::Tone5:
        lM = Tone5;
        break;

    // UI
    case listMelody::Ok:
        lM = Ok;
        break;

    case listMelody::Cancel:
        lM = Cancel;
        break;

    case listMelody::Error:
        lM = Error;
        break;

    case listMelody::Click:
        lM = Click;
        break;

    default:
        break;
    }
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

    u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    u8g2.setBitmapMode(0);// 0-off, 1-on Transporent mode
    u8g2.setColorIndex(1);// 0-off px, 1-on px

    if ((_joy.posX0 >= xForm && _joy.posX0 <= (xForm + (sizeText * charW) + border + border)) && (_joy.posY0 >= yForm && _joy.posY0 <= yForm + 13))
    {

        u8g2.setColorIndex(1);                                               // включаем пиксели
        u8g2.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13);    // рисуем черный бокс
        u8g2.setColorIndex(0);                                               // отключаем птксели
        _gfx.print(m_label, xForm + border, yForm + 7 /* font H */ + border, 8, charW); // выводим тест
        u8g2.setColorIndex(1);                                               // включаем пиксели

        if (_joy.pressKeyENTER() == true)
        {
            m_stateButton = true;
            m_func(); 
        }
        else m_stateButton = false;
    }
    else
    {
        u8g2.setColorIndex(1);                                                  // включаем пиксели
        u8g2.drawFrame(xForm, yForm, (sizeText * charW) + border + border, 13);     // рисуем прозрачный фрейм
        _gfx.print(m_label, xForm + border, yForm + 7 /* font H */ + border, 8, charW); // выводим тест
    }
}
/* eText */
void eText::show()
{
    _gfx.print(m_text, xForm, yForm + highChar, 10, 5);
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
        u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
    }
    if (m_borderStyle == twoLine)
    {
        u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
        u8g2.drawFrame(xx - 3, yy - 3, m_sizeW + 6, m_sizeH + 6);
    }
    if (m_borderStyle == shadow)
    {
        u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);

        u8g2.drawHLine(xx + 1, yy + m_sizeH, m_sizeW);
        u8g2.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW);

        u8g2.drawVLine(xx + m_sizeW, yy + 1, m_sizeH);
        u8g2.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH);
    }
    if (m_borderStyle == shadowNoFrame)
    {
        u8g2.drawHLine(xx + 1, yy + m_sizeH, m_sizeW);
        u8g2.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW);

        u8g2.drawVLine(xx + m_sizeW, yy + 1, m_sizeH);
        u8g2.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH);
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
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(xx + border, yy + charH + border);
        u8g2.print(c);

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

    u8g2.setColorIndex(1); // включаем пиксели

    u8g2.setCursor(x, y);
    u8g2.setFont(u8g2_font_6x10_tr);

    for (int i = 0; i < sizeText; i++)
    {
        u8g2.setCursor(x, y);
        u8g2.print(m_text[i]); x += chi;

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
    uint8_t yy{}, chi{5}, lii{8}; int x{xForm + 1}, y{yForm + 8};

    if ((_joy.posX0 >= x && _joy.posX0 <= (x + (sizeText * chi))) && (_joy.posY0 >= y - (lii + 2) && _joy.posY0 <= y + 2))
    {

        u8g2.setColorIndex(1);                                         // включаем пиксели
        u8g2.drawBox(x - 1, y - (lii), (sizeText * chi) + 2, lii + 1); // рисуем черный бокс
        u8g2.setColorIndex(0);                                         // отключаем пиксели

        u8g2.setCursor(x + 3, y);
        u8g2.setFont(u8g2_font_6x10_tr);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            u8g2.setCursor(xx + x, yy + y);
            u8g2.print(m_text[i]);

            if (m_text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
        u8g2.setColorIndex(1);                                          // включаем пиксели


        if (_joy.pressKeyENTER() == true)
        {
            m_onClick();
        }
    }
    else
    {
        u8g2.setColorIndex(1);                                          // включаем пиксели

        u8g2.setCursor(x + 3, y);
        u8g2.setFont(u8g2_font_6x10_tr);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            u8g2.setCursor(xx + x, yy + y);
            u8g2.print(m_text[i]);

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
    u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    u8g2.setBitmapMode(0);// 0-off, 1-on Transporent mode
    u8g2.setColorIndex(1);// 0-off px, 1-on px
    u8g2.drawHLine(xForm, yForm, wForm);
}
/* eCheckbox */
void eCheckbox::show()
{
    // рисуем фрейм и выводим текст
    if (m_checked == true)
    {
        u8g2.drawXBMP(xForm, yForm, check_true_w, check_true_h, check_true);
    }

    if (m_checked == false)
    {
        u8g2.drawXBMP(xForm, yForm, check_false_w, check_false_h, check_false);
    }

    _gfx.print(m_text, xForm + 15, yForm + 9, 10, 5);
    // если курсор над фреймом, то ждем нажатия на кнопку Ввода
    if ((_joy.posX0 >= xForm) && (_joy.posX0 <= xForm + 10) && ((_joy.posY0 >= yForm) && (_joy.posY0 <= yForm + 10)))
    {
        u8g2.drawBox(xForm, yForm, 10, 10);

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

        u8g2.setColorIndex(1); // вкл пиксели
        u8g2.drawFrame(0, 12 /* height button (13px) - 1 */, 256, 148); // x, y, w, h
        _gfx.print(10, title, 5, 10, 10, 5); // size Font, text, x, y, lii, chi
        
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 205;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 200;
        }
        
        _gfx.print(10, "[" + (String)sizeStack + "]", xSizeStack, 10, 10, 5);
    }

    if (eFormShowMode == MAXIMIZED)
    {
        if (closeForm.button2("CLOSE", 225, 0, _joy.posX0, _joy.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        u8g2.setColorIndex(1); // вкл пиксели
        u8g2.drawFrame(0, 12 /* height button - 1 */, 256, 137 /* 137 - 10px tray*/); // x, y, w, h
        _gfx.print(10, title, 5, 10, 10, 5); // size Font, text, x, y, lii, chi
       
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 205;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 200;
        }
       
       _gfx.print(10, "[" + (String)sizeStack + "]", xSizeStack, 10, 10, 5);
    }

    if (eFormShowMode == NORMAL)
    {

        if (closeForm.button2("CLOSE", 205, outerBoundaryForm - 12 + 6, _joy.posX0, _joy.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        u8g2.setColorIndex(1); // вкл пиксели
        u8g2.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120); // x, y, w, h
        _gfx.print(10, title, outerBoundaryForm + 5, outerBoundaryForm - 2 + 6, 10, 5);
        
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 185;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 180;
        }

        _gfx.print(10, "[" + (String)sizeStack + "]", xSizeStack, outerBoundaryForm - 6 + 10, 10, 5);
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
    u8g2.setContrast(m_valueContrast);
    // Serial.println(m_valueContrast);
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
    
    if (isTouched() == true)
    {
        screenTiming = TIMER; u8g2.setPowerSave(0);  //off powersave
    }
    
    if ((TIMER - screenTiming > 60000) && (_joy.posY0 >= 150))
    {
        screenTiming = TIMER;

        while (isTouched() == false)
        {
            InstantMessage message1("The console has entered\ndeep sleep mode.", 3000);
            message1.show();

            systems.setBacklight(false);
            u8g2.setPowerSave(1);         //off display
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
            u8g2.setPowerSave(0);          //on display
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
/* Draw dot-grid */
void InstantMessage::drawDotGrid(int interval)
{
    // Проверяем, чтобы интервал был больше 0
    if (interval <= 0)
    {
        return; // Если интервал некорректный, ничего не делаем
    }

    // Проходим по всем строкам и столбцам с заданным интервалом
    for (int x = 0; x < 256; x += interval)
    {
        for (int y = 0; y < 160; y += interval)
        {
            u8g2.drawPixel(x, y); // Рисуем точку (1 — белый цвет)
        }
    }
}
/* Draw show function */
void InstantMessage::show()
{
    short border{5}, border2{8}, charHeight{10}, charWidth{5};
    int count{0}, x{128}, y{80}, maxChar{0}, line{1};

    for (char c : m_text) // count the characters in each line
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

    int lineYoffset = (line / 2) * 8;
    
    //--> Рисуем сетку на дисплее
    u8g2.setDrawColor(1);
    drawDotGrid(2); //1, 2, 4, 8, 16, 32,
    
    //--> выводим границы текста
    int numberOfPixels = maxChar * charWidth;               // the maximum number of pixels based on the number of characters in a line
    int numberOfPixelsToOffset = (maxChar / 2) * charWidth; // a given number of pixels must be offset

    //--> границы фрейма
    // Определяем границы фрейма
    int frameX = x - numberOfPixelsToOffset - border2 + GLOBAL_X;
    int frameY = y - charHeight - border2 + GLOBAL_Y - lineYoffset;
    int frameWidth = border2 + border2 + numberOfPixels;
    int frameHeight = border2 + border2 + (line * charHeight);
    // Проходим по всем пикселям внутри фрейма и рисуем их
    u8g2.setDrawColor(0);
    for (int px = frameX; px < frameX + frameWidth; px++)
    {
        for (int py = frameY; py < frameY + frameHeight; py++)
        {
            u8g2.drawPixel(px, py); // Рисуем пиксель со значением 0
        }
    }
    u8g2.setDrawColor(1);
    //<-- границы фрема

    // u8g2.clearBuffer(); // -->
    _gfx.print(m_text, x - numberOfPixelsToOffset + GLOBAL_X, y + GLOBAL_Y - lineYoffset, charHeight, charWidth);

    u8g2.drawFrame(x - numberOfPixelsToOffset - border + GLOBAL_X, y - charHeight - border + GLOBAL_Y - lineYoffset, border + border + numberOfPixels, border + border + (line * charHeight));
    u8g2.drawFrame(x - numberOfPixelsToOffset - border2 + GLOBAL_X, y - charHeight - border2 + GLOBAL_Y - lineYoffset, border2 + border2 + numberOfPixels, border2 + border2 + (line * charHeight));

    u8g2.sendBuffer(); // <--

    delay(m_delay);
}






//====================================================

//1
void _graphicsTest1(int xG, int yG, int wG, int hG)
{   
    for (int x = 0; x < 256; x += 2)
    {
        int y = random(12, 160);
        int h = random(160 - y);
        u8g2.drawVLine(x, y, h); 
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
                u8g2.drawBox(x, y, w, h);
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
        u8g2.drawPixel(points[i][0], points[i][1]);
    }

    // draw lines
    for (int i = 0; i < numPoints; i++)
    {
        for (int j = i + 1; j < numPoints; j++)
        {
            u8g2.drawLine(points[i][0], points[i][1], points[j][0], points[j][1]);
        }
    }
}
void _myGraphicsTest3()
{
    exForm *formGraphicsTest3 = new exForm;
    eGraphics *graphicsTest3 = new eGraphics(_graphicsTest3, 0, 0, 256, 147);

    formGraphicsTest3->title = "Graphics test 3";
    formGraphicsTest3->eFormShowMode = FULLSCREEN;
    formGraphicsTest3->addElement(graphicsTest3);

    formsStack.push(formGraphicsTest3);
}


//====================================================
/* Desktop */
void _myDesktop()
{
    exForm *form0 = new exForm();

    eDesktop<TaskArguments> *desktop0 = new eDesktop<TaskArguments>(tasks);

    form0->title = "Desktop";
    form0->eFormShowMode = FULLSCREEN;

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
    ePicture *pic1 = new ePicture(gigachat_bits, 160, 100, gigachat_w, gigachat_h);

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

    ePicture *pic1 = new ePicture(alisa_5050_bits, 10, 35, alisa_5050_w, alisa_5050_h);
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
    eButton *button1 = new eButton("Test message", [](){
        InstantMessage message0("Hello world,\nhello, hello, hello\nw o r l d!\0", 3000);
        message0.show();
    }, 5, 5);

    form1->title = "Form 1";
    form1->eFormShowMode = NORMAL;
    form1->addElement(button1);


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

    u8g2.setDrawColor(1); // 0-white,  1-black, 2-XOR
    u8g2.setColorIndex(1);// 0-off px, 1-on px

    // _gfx.print("Active tasks:", xx + 5, yy + 10, 8, 5);

    for (auto &_ta : tasks)
    { 
        if (_ta.activ == true)
        {
            String _Text = _ta.name;
            uint8_t sizeText = _Text.length();

            _gfx.print(_ta.name, xx, yy, 8, 5);
            
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
            labelTask.label(_ta.name, _ta.f, xx, yy);
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
    eLabel *label1 = new eLabel((String)systems.VALUECONTRAST, x + 26, y + 30);

    eButton *button1 = new eButton("-", []()
                                   { systems.VALUECONTRAST -= 1; delay(250); systems.setDisplayContrast(systems.VALUECONTRAST); }, x + 5, y + 20);
    eButton *button2 = new eButton("+", []()
                                   { systems.VALUECONTRAST += 1; delay(250); systems.setDisplayContrast(systems.VALUECONTRAST); }, x+ 50, y + 20);

    eFunction *func1 = new eFunction([label1]()
                                     { label1->setText((String)systems.VALUECONTRAST); });

    eLabel *label2 = new eLabel("display contrast", x + 66, y + 30);

    // --> RAW data Stick0
    /* ! нужно захватить эти переменные по ссылке, используя &. */
    // String rawDataStick;
    // eFunction *func2 = new eFunction([&rawDataStick](){
    //     rawDataStick = "RAW data Stick: X: " + String(_joy.RAW_DATA_X0) + " Y: " + String(_joy.RAW_DATA_Y0);
    // });
    // eLabel *label3 = new eLabel(rawDataStick, x + 5, y + 45);
    
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
/* FPS */
void _myFps()
{
    int fps = _gfx.calculateFPS();
    u8g2.setCursor(241, 135);
    u8g2.print(fps);
}


/* Cursor */
void _systemCursor()
{
    _joy.updatePositionXY(20);
    _crs.cursor(true, _joy.posX0, _joy.posY0);

    if ((_joy.pressKeyEX() == true) && (_joy.pressKeyENTER() == true))
    {
        u8g2.setCursor(_joy.posX0 + 10, _joy.posY0 + 10);
        u8g2.print(_joy.posX0);
        u8g2.setCursor(_joy.posX0 + 10, _joy.posY0 + 20);
        u8g2.print(_joy.posY0);
    }
}

Icon icon;
/*
    Task-dispatcher
    Dispatcher tasks, vector
    [01/2025, Alexander Savushkin]
*/
TaskArguments system0[] //0 systems, 1 desktop, 2 user
{
    // {"powersave", _systemPowerSaveBoard, NULL, SYSTEM, 0, true}, // ! modify
    {"fps", _myFps, NULL, SYSTEM, 0, false},
    {"desktop", _myDesktop, NULL, SYSTEM, 100, true},
    {"oshello", _osHello, NULL, SYSTEM, 101, true},
    {"form1", _myForm1, icon.MyNullApp, DESKTOP, 0, false},
    {"form2", _myForm2, icon.MyNullApp, DESKTOP, 0, false},
    {"form3", _myForm3, icon.MyNullApp, DESKTOP, 0, false},
    {"graphics 1", _myGraphicsTest1, icon.MyGfx, DESKTOP, 0, false},
    {"graphics 2", _myGraphicsTest2, icon.MyGfx, DESKTOP, 0, false},
    {"dispatcher", _myDispatcher, icon.MyTaskManager, DESKTOP, 0, false},
    {"graphics 3", _myGraphicsTest3, icon.MyGfx, DESKTOP, 0, false},
    {"settings", _settingsForm, icon.MyTech, DESKTOP, 0, false},
    {"userdesktop", _userDesktop, icon.MyUserWorkSpace, DESKTOP, 0, false},
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