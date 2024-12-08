/*
  Contains function settings for working with the display.

  [!] Required u8g2 library
  [!] bmp to xbmp image converter https://www.online-utility.org/image/convert/to/XBM
                                  https://windows87.github.io/xbm-viewer-converter/
  [!] midi to arduino tones converter https://arduinomidi.netlify.app/
      ntp client                  https://github.com/arduino-libraries/NTPClient
*/

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

#include <iostream>
#include <Arduino.h>
#include <U8g2lib.h> 
#include "ex.h"
#include "ex_xbm.h"

#include <WiFi.h>
#include "NTPClient.h"
#include <WiFiUdp.h>
#include <vector>

//version Library and Text
const int8_t VERSION_LIB[] = {0, 0, 3};
String VERSION_ADD_INFORMATION = "";
String TEXT_UI_BEGIN = "The experience system. 2023-2024\nDev: Savushkin A, Ksenofontov S,\nSyatkina E, Samoilov M, ";

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
void clearCommandTerminal(); void testApp(); void myDesktop();
void myWifiConnect(); void myWifiDisconnect(); void sustemLedControl(); void flagLedControl();
void myTray();
void myEx(); void myExViewTaskList();
void _pushSystemsTask(); int _sizeTasks(); //vector only


//for screensaver
unsigned long screenTiming{}, screenTiming2{}, TIMER{};

//buffer -->use enum?
String BUFFER_STRING{};
int BUFFER_INT{};
double BUFFER_DOUBLE{};

//for timer
unsigned long previousMillis{};
unsigned long prevTime_0{};
const long interval{300};

/* Graphics chip setup */
U8G2_ST75256_JLX256160_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/5, /* dc=*/17, /* reset=*/16);

/* Liquid crystal display resolution. */
int H_LCD{160}, W_LCD{256};
/* Analog-to-digital converter resolution (Chip PICO 2040). */
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

/* Backlight */
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

/* Graphics */
/* graphics output objects */
void Graphics::initializationSystem()
{
    /* GPIO release from sleep */
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 1);
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 1);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1); // Stick 0
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); // Stick 0
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1); // EX button
    
    //setting the operating system state
    //setting display, contrast
    u8g2.begin(); Serial.begin(9600);
    u8g2.setContrast(143); //143//150
    //setting the resolution of the analog-to-digital converter
    analogReadResolution(RESOLUTION_ADC);
    //display backlight
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);
    //_gfx.controlBacklight(true);
    //PIN mode
    pinMode(PIN_BUTTON_ENTER, INPUT);
    pinMode(PIN_BUTTON_EX,    INPUT);
    pinMode(PIN_BUTTON_A,     INPUT);
    pinMode(PIN_BUTTON_B,     INPUT);
    pinMode(PIN_BATTERY,      INPUT);

    /* off-backlight */
    _gfx.controlBacklight(false);
    //platform logo output
    image_width = ex_width;
    image_height = ex_height; _pushSystemsTask(); int _ssize = _sizeTasks();
    //--
    u8g2.clearBuffer();
    u8g2.drawXBMP(((W_LCD - image_width)/2), ((H_LCD - image_height)/2) - 7, image_width, image_height, ex_bits);
    _gfx.print(10, TEXT_UI_BEGIN, 32, ((H_LCD/2) + (image_height/2) + 7), 10, 6);
    _gfx.print(6, (String)VERSION_LIB[0] + "." + (String)VERSION_LIB[1] + "." + (String)VERSION_LIB[2] + " " + VERSION_ADD_INFORMATION, 0, H_LCD, 10, 4);
    
    _gfx.print(6, (String)_ssize, 0, 6, 10, 6);
    u8g2.sendBuffer();
    //--

    delay(2500);


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


/* Print */
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

/* Cursor */
/* displaying the cursor on the screen */
bool Cursor::cursor(bool stateCursor, int xCursor, int yCursor)
{
    if (stateCursor == true)
    {
        u8g2.setDrawColor(2);
        u8g2.setBitmapMode(1);
        u8g2.drawXBMP(xCursor, yCursor, cursor_w, cursor_h, cursor_bits);
        u8g2.setDrawColor(1);
        u8g2.setBitmapMode(0);
        return true;
    }
    else
        return false;
}


/* Interface */
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


/* Button */
/* Button return boolean state */
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
/* Button return boolean state */
bool Button::button(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor)
{
  uint8_t sizeText = text.length();

  if ((xCursor >= x && xCursor <= (x + (sizeText * 5) + 4)) && (yCursor >= y - 8 && yCursor <= y + 2))
  {
    u8g2.setDrawColor(1);
    u8g2.drawBox(x, y - 8, (sizeText * 5) + 5, 10);

    if (Joystick::pressKeyENTER() == true)
    {
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


/* Shortcut */
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


/* Label */
/*  */
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
/*  */
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


/* Joystic and Key */
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


/* Timer */
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


/* Song engine */
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
void null(){}


/* System task-function */
/* update NTP time */
void systemNTPTimeUpdate()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        timeClient.update();
    }
    else _mess.popUpMessage("!", "The Wi-Fi (internet) connection\nis not active.", 2500);
}

/* Task. Taskbar-area */
int xTray{256}, yTray{159}, borderTray{5};
void trayClock()
{
    //width 40px
    _labelClock.label((String)timeClient.getFormattedTime(), "Click to update time", xTray, yTray, systemNTPTimeUpdate, 8, 5, _joy.posX0, _joy.posY0);
}

/* Task. Draw IP connect */
void trayDrawIpConnect()
{
    //width 75px
    _labelWifi.label(WiFi.localIP().toString(), "Click to disconnect", xTray, yTray, myWifiDisconnect, 8, 5, _joy.posX0, _joy.posY0);
}
/* Task. Buffer */
















/* Task management */
/* disable a task */
void Task::taskKill(int indexTask)
{
    for (_taskArguments &_ta : _taskSystems)
    {
        if ((_ta.active == true) && (_ta.indexTask == indexTask))
        {
            _ta.active = false;
        }
    }
}
/* start a task */
void Task::taskRun(int indexTask)
{
    for (_taskArguments &_ta : _taskSystems)
    {
        if ((_ta.active == false) && (_ta.indexTask == indexTask))
        {
            _ta.active = true;
        }
    }
}
/* task */
void Task::task(int indexTask)
{
    for (_taskArguments &_ta : _taskSystems)
    {
        if ((_ta.active == true) && (_ta.indexTask == indexTask))
        {
            _ta.active = false; delay(150); break;
        }

        if ((_ta.active == false) && (_ta.indexTask == indexTask))
        {
            _ta.active = true; delay(150); break;
        }  
    }
}


/* Application */
/* window designer for the task-function */
void Application::window(String name, int indexTask, void (*f1)(void), void (*f2)(void))
{
    _task.taskKill(100); //kill Desktop
    _task.taskRun(indexTask);
    
    f1(); //calc
    f2(); //graphics
    
    //draw window
    {
        _gfx.print(name, 5, 9, 8, 5); u8g2.setDrawColor(1);
        u8g2.drawFrame(0, 10, 256, 141);
    }
    //draw button-state-window
    {
        //if (_collapse.button(" COLLAPSE ", 162, 9, _joy.posX0, _joy.posY0)) {}
        
        if (_close.button(" CLOSE ", 216, 9, _joy.posX0, _joy.posY0))
        {
            _task.taskKill(indexTask);
            _task.taskRun(100); //run Desctop
        }
    }
}
/* window designer for the task-function */
void Application::window(String name, int indexTask, void (*f1)(void))
{
    _task.taskKill(100); //kill Desktop
    _task.taskRun(indexTask);
    
    f1();
    
    //draw window
    {
        _gfx.print(name, 5, 9, 8, 5); u8g2.setDrawColor(1);
        u8g2.drawFrame(0, 10, 256, 141);
    }
    //draw button-state-window
    {
        //if (_collapse.button(" COLLAPSE ", 162, 9, _joy.posX0, _joy.posY0)) {}
        
        if (_close.button(" CLOSE ", 216, 9, _joy.posX0, _joy.posY0))
        {
            _task.taskKill(indexTask);
            _task.taskRun(100); //run Desctop
        }
    }
}




//====================================================
/* Application */
/* my tray */
void _myTray()
{
    u8g2.setDrawColor(1);
    u8g2.drawHLine(0, 150, 256); 
    xTray = 256;
    
    for (_taskArguments &_ta : _taskSystems)
    {
        if (_ta.state == 3)
        {
            if (_ta.active == true)
            {
                //xTray += borderTray;
                xTray -= borderTray + _ta.widthApp;
                _ta.f();
                //xTray -= command.widthApp;
            }
        }
    }
}
/* my desctop */
void _myDesktop()
{
    uint8_t border{4};
    uint8_t xx{border};
    uint8_t yy{15}; 

    uint8_t countTask{1};
    
    for (_taskArguments &_ta : _taskSystems)
    {
        if (_ta.state == 2)
        {
            _shortcutDesktop.shortcut(_ta.name, _ta.bitMap, xx, yy, _ta.f, _joy.posX0, _joy.posY0);
            countTask++;

            xx += (32 + border);

            if (countTask > 7) 
            {
                xx = 4; yy += (32 + border); countTask = 0;
            }
        }
    }
    
    u8g2.drawXBMP(56, 0, grayLine_width, grayLine_height, grayLine_bits);
    _gfx.print("My Desktop", 5, 8, 8, 5);
    u8g2.drawHLine(0, 10, 256);
}
/* Task. Stack, task, command */
void _myConsole()
{
    _mess.popUpMessage("!", "Ohhh no :(\nTask-function not defined!\0", 5000);
    _joy.resetPositionXY();
}
/* Data Port */
/* list all port's */
struct _dataPort
{
    short number;
    short gpio;
    short state; //0x01(1) - input 0x00(0) - output
    bool  activ;
};

_dataPort _allDataPort[]
{
    /*
    Data port
    [1] GND [2] 3,3V
    [3] GPIO 35 [4] GPIO 34 [5] GPIO 19
    [6] TX [7] RX [8] GPIO 21 [9] GPIO 22
    */
    
    {3, 35, 1, false}, //adc
    {4, 34, 1, false}, //adc
    {5, 19, 1, false}, //miso

    {6, 1,  0, false}, //tx output UART
    {7, 3,  1, false}, //rx input UART

    {8, 21, 1, false}, //sda
    {9, 22, 1, false}  //scl
};

bool _stateInstallPort = false;
void _installingPort()
{
    if (!_stateInstallPort)
    {
        for (_dataPort _dp : _allDataPort)
        {
            pinMode(_dp.gpio, _dp.state);
        }
    }
}

void _f2()
{
    /*for(_dataPort _dp : _allDataPort)
    {
        if ((_dp.gpio == 35) || (_dp.gpio == 34))
        {
            _dataDp = analogRead(_dp.gpio);
        }
    }*/

    _gfx.print("DataPort 4 " + (String)analogRead(34), 10, 25, 10, 5);
    _gfx.print("DataPort 3 " + (String)analogRead(35), 10, 35, 10, 5);
}
void _myDataPort()
{
    bool stateTaskMyDataPort = _mess.dialogueMessage("Data port", "Pinout\n[1] GND [2] 3,3V\n[3] GPIO 35 [4] GPIO 34 [5] GPIO 19\n[6] TX [7] RX\n[8] GPIO 21 [9] GPIO 22\n\nMake a connection?");
    if (stateTaskMyDataPort == true)
    {
        _app.window("Data port", 102, _f2);
    }
    if (stateTaskMyDataPort == false) _task.taskKill(102);

    _joy.resetPositionXY();
}
/* Test Application. Function */
void _f1()
{
    String text = "Application class test.\n\n_app.window(Test Application, 103, f1);\n_app - object\nwindow - class function\nTest Application - name window\n103 - number task\nf1 - function";
    _gfx.print(text, 10, 25, 10, 5);
}
/* Test application. Call object */
void _myTestApp()
{
    _app.window("Test Application", 103, _f1);
}
/* Task Manager */
/* my Task Manager. View the task list */
int _numberIndexTask{};
void _changingStatusTask()
{
    _task.task(_numberIndexTask);
}

void _dialogueChangingStatusTask()
{
    String textDialogue = "Click OK to start or stop\nthe process: " + (String)_numberIndexTask;
    bool state = _mess.dialogueMessage("?", textDialogue);

    if (state == true) {_changingStatusTask();}
    if (state == false) {}
}

void _viewTaskList()
{
    int xx{5}, yy{30};

    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);

    _gfx.print("Active tasks:", 5, 20, 8, 5);

    for (_taskArguments &_ta : _taskSystems)
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
    
    for (_taskArguments &_ta : _taskSystems)
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
    }
}

void _myTaskManager()
{
    _app.window("View the task list", 105, _viewTaskList, null);
}
/* Clear all commands */
void _clearCommandTerminal()
{
  for (_taskArguments &_ta : _taskSystems)
  {
    _ta.active = false;
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
void _powerSaveBoard()
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
/* User workspace */
/* Function */
void _userWorkSpace(){}
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









/* my wi-fi */
void myWifiDisconnect()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    //_mess.popUpMessage("!", "Wi-Fi is disabled!\0", 2000);
    
    _task.taskKill(104); _task.taskKill(205);
    stateWifiSetup = false; stateWifi = false;
}
/* my wi-fi */
void myWifiConnect()
{
    _task.taskRun(104);
    
    {
        //add network selection
    }

    if (stateWifiSetup == false)
    {
        //WiFi.begin("Allowed", "Serjant1985"); stateWifiSetup = true;
        WiFi.begin("RT-GPON-6089", "u7PxRkFQ"); stateWifiSetup = true;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        BUFFER_STRING = "Wi-Fi...";
        stateWifi = false;
        _stop.stopwatch(myWifiDisconnect, 10000);
    }
    else
    {
        stateWifi = true;
        _task.taskRun(205);
        //_gfx.print(WiFi.localIP().toString(), 130, 159, 8, 5);
        //_disconnect.button("X", 115, 158, myWifiDisconnect, _joy.posX0, _joy.posY0);
    }

    /* IPAddress ip = WiFi.localIP();
    sprintf(lcdBuffer, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], udpPort);*/
}


//===========================================================================


/* Development 2 */
/* vector */

_taskArguments _systems[]
{
    /* system task */
    {"clearcomm",   "Clear command",       _clearCommandTerminal, false,     0, NULL, 0, 0, 0},
    {"deepsleep",   "Deep sleep PWS-mode", _powerSaveBoard,       true,      1, NULL, 0, 0, 0},
    {"rawadc",      "Raw data ADC",        _systemRawADC,         false,     2, NULL, 0, 0, 0},
    {"clearbuffer", "Clear Buffer",        _clearBufferString,    false,     3, NULL, 0, 0, 0},
    {"reboot",      "Reboot board",        _rebootBoard,          false,     4, NULL, 0, 0, 0},
};

_taskArguments _desktop[]
{
    /* desktop task. workspace */
    {"mydesctop",    "My Desctop",          _myDesktop,            true,    100, NULL,                   0, 0, 1},
    /* app's */
    {"myconsole",    "My Console",          _myConsole,            false,   101, icon_MyConsole_bits,    0, 0, 2},
    {"myserialport", "My Data port",        _myDataPort,           false,   102, icon_MyDataPort_bits,   0, 0, 2},
    {"testapp",      "Test Application",    _myTestApp,            false,   103, icon_MyNullApp_bits,    0, 0, 2},
    //{"mywifi",      "My WiFi",             myWifiConnect,        false,   104, iconMyWiFiClient_bits, 0, 0, 2},
    {"mytaskmanager","My Task manager ",    _myTaskManager,        false,   105, icon_MyTaskManager_bits,0, 0, 2},
    //{"dataportapp",  "Data port App",       _myDataPortApp,        false,   106, NULL,                   0, 0, 0},
};

_taskArguments _user[]
{
    {"userworkspace","User workspace",     _userWorkSpace,        false,  400, icon_UserWorkSpace_bits, 0, 0, 2},
};

_taskArguments _tray[]
{
    /* taskbar-area */
    //clear tray
    //{"cleartray", "Clear Tray",    null,         false, 200, NULL, 0,  0, 3},
    //{"clock",      "Clock",                trayClock,          false, 201, NULL, 40, 0, 3},
    {"battery",    "Battery control",      _trayBattery,        true,  202, NULL, 15, 0, 3},
    {"fps",        "FPS",                  _trayFps,            true,  203, NULL, 10, 0, 3},
    //{"ip",         "Ip adress",            trayDrawIpConnect,  false, 205, NULL, 75, 0, 3},
    {"buffer",     "Buffer",               _trayBuffer,         true,  204, NULL, 0,  0, 3},
};

_taskArguments _systems2[]
{
    /* system graphics-task */
    //keyboard task
    //{"", "", NULL, false, 298, NULL, 0, 0, 0},
    {"sysledcontrol", "LED control",       _sustemLedControl,     true,    299, NULL, 0, 0, 0},
    {"systray",       "Tray",              _myTray,               true,    300, NULL, 0, 0, 0},
    {"syscursor",     "Cursor",            _systemCursor,         true,    301, NULL, 0, 0, 0},
};

void _addSystemsTask(const _taskArguments& a)
{
    _taskSystems.push_back(a);
}

void _kernelSystemsTask()
{
    for(_taskArguments &_ta : _taskSystems)
    {
        if ((_ta.active) && (_ta.state != 3)) _ta.f();
    }
}

int _sizeTasks()
{
    int _size = _taskSystems.size();
    return _size;
}

void _pushSystemsTask()
{
    for(_taskArguments &_all : _systems)
    {
        _taskSystems.push_back(_all);
    }

    for(_taskArguments &_all : _desktop)
    {
        _taskSystems.push_back(_all);
    }

    for(_taskArguments &_all : _user)
    {
        _taskSystems.push_back(_all);
    }

    for(_taskArguments &_all : _tray)
    {
        _taskSystems.push_back(_all);
    }

    for(_taskArguments &_all : _systems2)
    {
        _taskSystems.push_back(_all);
    }
}

void Terminal::terminal2()
{
  TIMER = millis();
  
  _gfx.render(_kernelSystemsTask);

  if (Serial.available() != 0)
  {
    char text[20]{};
    Serial.readBytesUntil('\n', text, sizeof(text));

    for (_taskArguments &_ta : _taskSystems)
    {
      if (not strncmp(_ta.text, text, 20))
      {
        if (_ta.active == true) _ta.active = false;
        else _ta.active = true;
      }
    }
  }
}







//=================================================================================