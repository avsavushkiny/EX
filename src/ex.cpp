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

#include <WiFi.h>
#include "NTPClient.h"
#include <WiFiUdp.h>

#include <vector>
#include <functional>
#include <algorithm>

//version Library and Text
const int8_t VERSION_LIB[] = {0, 0, 3};

Graphics _gfx; 
Timer _delayCursor, _trm0, _trm1, _stop, _timerUpdateClock, _fps; 
Application _app; 
Joystick _joy; 
Shortcut _shortcutDesktop, _wifi;
Cursor _crs; 
PowerSave _pwsDeep; 
Interface _mess; 
Button _ok, _no, _collapse, _expand, _close, _ledControl;
TimeNTP _timentp; Task _task;
Label _labelClock, _labelBattery, _labelWifi, _taskList;
TextBox _textBox;

TaskDispatcher td;

/* WIFI */
bool stateWifiSetup = false;
bool stateWifi = false;

/* LED control */
bool systemStateLedControl = true; bool flagStateLedControl = false;

/* Time NTP*/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.apple.com", 10800, 60000);

/* Prototype function */
void null();
//vector only
void addTasksForSystems();

//for screensaver
unsigned long screenTiming{}, screenTiming2{}, TIMER{};

//buffer -->use enum?
String BUFFER_STRING{""}; //Hi I'm EX OS, Experience board!
int BUFFER_INT{};
double BUFFER_DOUBLE{};

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


/* 
    class
    Graphics
*/
/* backlight */
bool Graphics::controlBacklight(bool state) //p-n-p transistor
{
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);

    if (state == true)
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 0); // on
        return true;
    }
    else
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 1); // off
        return false;
    }
}
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
    
    /* setting display, contrast */
    u8g2.setContrast(143); //143//150

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
    _gfx.controlBacklight(false);

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
void Interface:: popUpMessage(String label, String text, uint tDelay)
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

    _gfx.print(label, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a - (border + 4));
    _gfx.print(text, (W_LCD/2)-(maxChar*6)/2, (H_LCD/2) - a + 10);
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

  if ((xCursor >= x && xCursor <= (x + (sizeText * charW) + border + border)) && (yCursor >= y && yCursor <= y + 13))
  {
    u8g2.drawBox(x, y, (sizeText * charW) + border + border, 13);

    if (Joystick::pressKeyENTER() == true)
    {
      return true;
    }
  }
  else
  {
    u8g2.drawFrame(x, y, (sizeText * charW) + border + border, 13);
  }

  //u8g2.setCursor(x + border, y + 7 /* font H */ + border);
  //u8g2.setFont(u8g2_font_6x10_tr);

  u8g2.setFontMode(1);

  u8g2.setDrawColor(2);
  _gfx.print(text, x + border, y + 7 /* font H */ + border, 8, charW);
  u8g2.setDrawColor(1);
  
  u8g2.setFontMode(1);
  
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
  //u8g2.drawXBMP(x, y + 24, 8, 8, icon_bits);
  u8g2.drawXBMP(x, y + 21, 11, 11, shortcut_bits);

  if ((xCursor >= x && xCursor <= (x + 32)) && (yCursor >= y && yCursor <= (y + 32)))
  {
    u8g2.drawFrame(x, y, 32, 32);
    
    BUFFER_STRING = name;
    
    if (Joystick::pressKeyENTER() == true)
    {
      _gfx.waitDisplay();
      f();
      return true;
    }
  }
  else
  {
    
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

        BUFFER_STRING = name;

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

        BUFFER_STRING = text;

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
bool Label::label(String text, String description, uint8_t x, uint8_t y, void (*f)(void), uint8_t lii, uint8_t chi, int xCursor, int yCursor)
{
    uint8_t sizeText = text.length();
    uint8_t yy{};

    if ((xCursor >= x && xCursor <= (x + (sizeText * chi))) && (yCursor >= y - (lii + 2) && yCursor <= y + 2))
    {
        u8g2.setDrawColor(1);//1
        u8g2.drawBox(x - 1, y - (lii), (sizeText * chi) + 2, lii + 1);

        BUFFER_STRING = description;

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
    u8g2.setDrawColor(2);//2
    
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

    u8g2.setFontMode(0); //0-activate not-transparent font mode

    return false;
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




/* 
    class
    Form
*/
/* форма вывода сообщения */
void Form0::form(String title, String text, objectLocationForm location)
{
    TextBox _textBoxForm; Button _closeForm;

    while (1)
    {
        u8g2.clearBuffer(); // -->

        if (_closeForm.button2("CLOSE", 205, outerBoundaryForm - 12 + 6, _joy.posX0, _joy.posY0))
        {
            break;
        }

        u8g2.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120);

        _gfx.print(10, title, outerBoundaryForm + 5, outerBoundaryForm - 1 + 6, 10, 5);
        _textBoxForm.textBox(text, _textBoxForm.noBorder, 100, 196, 10, 5, outerBoundaryForm + innerBoundaryForm, outerBoundaryForm + innerBoundaryForm + 6);

        // cursor
        _joy.updatePositionXY(20);
        _crs.cursor(true, _joy.posX0, _joy.posY0);
        
        u8g2.sendBuffer(); // <--
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
    PowerSave
*/
/* turns off the backlight and turns on an infinite loop
   with the text to pause until the joysticks are pressed or moved */
bool _isTouched(); void _sleepModeScreen();//prototype
/* ---> remove support */
void PowerSave::sleepLight(bool state, uint timeUntil)
{
  if ((state == true))
  {
    if (_isTouched() == true)
    {
      screenTiming = millis();
    }

    if (millis() - screenTiming > timeUntil)
    {
      screenTiming = millis();

      digitalWrite(PIN_BACKLIGHT_LCD, false);

      while (_isTouched() == false)
      {
        /* Sleep */
        _gfx.render(_sleepModeScreen, 500);
        //esp_deep_sleep_start();
        esp_light_sleep_start();
      }

      digitalWrite(PIN_BACKLIGHT_LCD, true);
    }
  }
}
/* ---> remove support */
void PowerSave::sleepDeep(bool state, uint timeUntil)
{
  if ((state == true))
  {
    if (_isTouched() == true)
    {
      screenTiming = TIMER;
    }
    else screenTiming = screenTiming;


    if (TIMER - screenTiming > timeUntil)
    {
      screenTiming = TIMER;

      digitalWrite(PIN_BACKLIGHT_LCD, false);

      while (_isTouched() == false)
      {
        /* Sleep */
        esp_deep_sleep_start();
      }

      digitalWrite(PIN_BACKLIGHT_LCD, true);
    }
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


/* Task. Taskbar-area */
int xTray{256}, yTray{159}, borderTray{5};


// debug Form1
void TextMessage1::display1() const
{
    _gfx.print(m_text, m_x, m_y);
}






//====================================================
/* Application */
/* my tray */
void _myTray()
{
    u8g2.setDrawColor(1);
    u8g2.drawHLine(0, 150, 256);
    
    //u8g2.setDrawColor(2);
    //u8g2.drawBox(0,150,256,10);


    xTray = 256;
    
    for (TaskArguments &_ta : tasks)
    {
        /*if (_ta.state == 3)
        {
            if (_ta.active == true)
            {
                //xTray += borderTray;
                xTray -= borderTray + _ta.widthApp;
                _ta.f();
                //xTray -= command.widthApp;
            }
        }*/
    }

    u8g2.setDrawColor(1);
}
/* my desctop */
void _myDesktop()
{
    uint8_t border{4};
    uint8_t xx{border};
    uint8_t yy{15}; 

    uint8_t countTask{1};
    
    for (TaskArguments &t : tasks)
    {
        if ((t.activ == false) && (t.bitMap != NULL) && (t.type == 1))
        {
            _shortcutDesktop.shortcut(t.name, t.bitMap, xx, yy, t.f, _joy.posX0, _joy.posY0);
            countTask++;

            xx += (32 + border);

            if (countTask > 7) 
            {
                xx = 4; yy += (32 + border); countTask = 0;
            }
        }
        /*else
        {
            Form form1;
            form1.form("Notice", "There are no tasks to output to the desktop.", form1.itself);
        }*/
    }
    
    _gfx.print("My Desktop", 5, 8, 8, 5);

    u8g2.drawHLine(0, 10, 256);
}
/* Task. Stack, task, command */ 
void _myTablet()
{
    //Form0 _form;
    //_form.form("Hello friends!", "Platform Sozvesdiye\nCreate by Alexksander Savushkin\n01/2025", _form.itself);

    Form1 form1;
    form1.addTextMessage1("hello", 10, 20);
    form1.addTextMessage1("hello2", 10, 30);

    form1.displayForm1();

}
/* Data Port */
/* list all port's */
/* Test Application. Function */
/* Test application. Call object */
void _myTestApp()
{

}
/* Task Manager */



void _viewTaskList()
{
    /*int xx{5}, yy{30};

    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);

    _gfx.print("Active tasks:", 5, 20, 8, 5);

    for (TaskArguments &_ta : tasks)
    { 
        if (_ta.active == true)
        {
            String _Text = _ta.text;
            uint8_t sizeText = _Text.length();

            _numberIndexTask = _ta.indexTask;

            _taskList.label(_Text, _ta.name, xx, yy, _dialogueChangingStatusTask, 8, 5, _joy.posX0, _joy.posY0);
            
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
    
    _gfx.print("Inactive tasks:", 5, (yy+20), 8, 5);

    yy += 30;
    
    for (TaskArguments &_ta : tasks)
    { 
        if (_ta.active == false)
        {
            String _Text = _ta.text;
            uint8_t sizeText = _Text.length();

            _numberIndexTask = _ta.indexTask;
            
            _taskList.label(_Text, _ta.name, xx, yy, _dialogueChangingStatusTask, 8, 5, _joy.posX0, _joy.posY0);
            
            if ((xx + (sizeText * 5) + 5) <= 240) //256 - 5 - 5
            {
                xx += (sizeText * 5) + 5;
            }
            
            if ((xx + (sizeText * 5) + 5) >= 240)
            {
                xx = 5; yy += 10;
            }
        }
    }*/
}

void _myTaskManager()
{
    
}
/* Clear all commands */
void _clearCommandTerminal()
{
  for (TaskArguments &_ta : tasks)
  {
    _ta.activ = false;
  }
}
/* Powersave mode */
/* the function checks whether the joystick or button is pressed at a certain moment */
bool _isTouched()
{
  if ((_joy.calculateIndexY0() == 0) && (_joy.calculateIndexX0() == 0)) return false;

  return true;
}
/* shows a notification about the start of sleep mode */
void _sleepModeScreen()
{
    _mess.popUpMessage("Power save", "Light sleep.\nBye, bye my User!\nUse the Joystick to wake up!\0\0", 1000);
}
/* a system-task for working in an energy-efficient mode */
void _systemPowerSaveBoard()
{
    if (_isTouched() == true)
    {
        screenTiming = TIMER; u8g2.setPowerSave(0);  //off powersave
    }
    
    //if ((_joy.posY0 >= 150) && (_joy.posX0 <= 100)) BUFFER_STRING = "Deep powersave mode";
    
    if ((TIMER - screenTiming > 60000) && (_joy.posY0 >= 150))
    {
        screenTiming = TIMER;

        while (_isTouched() == false)
        {
            _sleepModeScreen();            //output message
            _gfx.controlBacklight(false); //off backlight
            u8g2.setPowerSave(1);         //off display
            esp_deep_sleep_start();       //run powersave, DEEP
        }
    }
    
    if ((TIMER - screenTiming > 60000) && (_joy.posY0 < 150))
    {
        screenTiming = TIMER;

        while (_isTouched() == false)
        {
            _sleepModeScreen();             //output message
            _gfx.controlBacklight(false);  //off backlight
            u8g2.setPowerSave(0);          //on display
            esp_light_sleep_start();       //run powersave, LIGHT
        }
    }
}
/* Task. System RawADC */
void _systemRawADC()
{
    String text = "Coord X: " + (String)_joy.RAW_DATA_X0 + " Coord Y: " + (String)_joy.RAW_DATA_Y0;
    BUFFER_STRING = text; 
}
/* Task. clear buffer */
void _clearBufferString()
{
    BUFFER_STRING = "";
}
/* Task. Reboot ESP32 */
void _rebootBoard()
{
    ESP.restart();
}

/* Tray */
/* buffer */
void _trayBuffer()
{
    //width 110px (22 chars)
    u8g2.setDrawColor(1);
    _gfx.print(BUFFER_STRING, 5, yTray, 8, 5);
    _trm0.timer(_clearBufferString, 100); //clear text-buffer
}
/* Battery */
int _t{};
int dataRawBattery{};
int _systemUpdateBattery()
{
    dataRawBattery = analogRead(PIN_BATTERY);
    dataRawBattery = map(dataRawBattery, 1890, 2470, 0, 100);

    return dataRawBattery;
}
int _systemBattery()
{
    if (_t >= 30000) _t = 30000;
    _timerUpdateClock.timer(_systemUpdateBattery, _t); _t += 10000;

    return dataRawBattery;
}
/* LED */
/* Task. System LED control */
void _flagLedControl()
{
    for (int i = 0; i < 1; i++)
    {
        if (flagStateLedControl == true)
        {
            flagStateLedControl = false; delay(250); break;
        }
        else
            flagStateLedControl = true; delay(250);
    }
}
void _sustemLedControl()
{
    //_ledControl.button("LED", 5, 140, flagLedControl, _joy.posX0, _joy.posY0);
    
    if (flagStateLedControl == true) _gfx.controlBacklight(true);
    else _gfx.controlBacklight(false);
}
void _trayBattery()
{
    //width 10px
    _labelBattery.label((String)_systemBattery(), "Click to LED on/off", xTray, yTray, _flagLedControl, 8, 5, _joy.posX0, _joy.posY0);
}
/* FPS */
uint8_t _FPS = 0; uint8_t _fpsCounter = 0; long int _fpsTime = millis();
void _trayFps()
{
    //width 10px
    _gfx.print((String)_FPS, xTray, yTray, 10, 6);
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
}
/* Cursor */
void _systemCursor()
{
    _joy.updatePositionXY(20);
    _crs.cursor(true, _joy.posX0, _joy.posY0);
}

/*
    Dev 3
    Vector + Dispatcher tasks
*/

TaskArguments system0[] //0 systems, 1 desktopTask
{
    {"systempowersave", _systemPowerSaveBoard, NULL, 0, 0, true},
    {"mydesktop", _myDesktop, NULL, 0, 0, true},
    {"myTablet", _myTablet, icon_mytablet_bits, 1, 0, false},
    // [!] Last task
    {"systemcursor", _systemCursor, NULL, 0, 0, true}
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
            t.activ == false; return true;
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

void runTasksCore()
{
    for (TaskArguments &t : tasks)
    {
        if (t.activ)
        {
            t.f();
        }
    }
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