/**
 * The MIT License (MIT)
 * Copyright (c) 2025 Alexander Savushkin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @mainpage "Experiment" console
 *
 * [!] Required u8g2 library
 * 
 * author: Alexander Savushkin
 * sub-authors: Sergey Ksenofontov
 * git:    @avsavushkiny
 * e-mail: avsavushkiny@live.ru
 * phone:  +7 (953) 034 4001
 */

#include <U8g2lib.h>

#ifndef EX_H
#define EX_H

/* We let the compiler know that the u8g2 object is defined in another file */
extern U8G2_ST75256_JLX256160_F_4W_HW_SPI u8g2;
extern const uint8_t gears_bits[];
extern int H_LCD, W_LCD;

extern void _clearCommandTerminal();

/*
    Dev3
    Dispatcher tasks
*/
/* Task Settings */
struct TaskArguments
{
    String name;           // Short task name
    void (*f)(void);       // Pointer to task function
    const uint8_t *bitMap; // Pointer to xbmp image
    int type;              // Task type, 0 - system, 1 - desktop
    int index;             // Task index
    bool activ;            // Task Status
};
/* Task Stack parameters */
struct TaskStack
{
    void (*f)(void);
    int index;
    int delay;
};
/* Task Manager Class */
class TaskDispatcher
{
public:
    // Determine the number of Tasks in the Vector
    int sizeTasks();
    // Add a task to Vector
    void addTask(const TaskArguments &task);
    // Remove task from Vector
    bool removeTaskVector(const String &taskName);
    // Make the Task inactive
    bool removeTask(const String &taskName);
    // Make the Task active
    bool runTask(const String &taskName);
    // Running all tasks from Vector
    void terminal3();
private:
};
/* Defining Vectors */
namespace
{
    std::vector<TaskStack> taskStack;     // Task Stack Vector
    std::vector<TaskArguments> tasks;     // Vector of main tasks
    std::vector<TaskArguments> tasksTray; // Notification bar task vector
};



/*
    Dev3
    Form1
*/
class fElement
{
public:
    virtual void fShow() const = 0;
};

class fButton : public fElement
{
public:
    fButton(const String& label, void (*onClick)(), int x, int y) : m_label(label), m_onClick(onClick), m_x(x), m_y(y) {}

    void fShow() const override;

    void fClick() //delete const in fShow
    {
        if (m_onClick != nullptr)
            m_onClick();
    }

private:
    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};

    String m_label;
    void (*m_onClick)(void);
    int m_x;
    int m_y;
};

class fText : public fElement
{
public:
    fText(const String& text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

    void fShow() const override;

private:
    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};

    short const highChar {10};

    String m_text;
    int m_x;
    int m_y;
};

class Form
{
private:
    std::vector<fElement*> m_elements;

    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};
public:
    ~Form()
    {
        for (fElement* element : m_elements)
        {
            delete element;
        }
    }

    void addButton(const String& label, void (*onClick)(), int x, int y)
    {
        m_elements.push_back(new fButton(label, onClick, x, y));
    }

    void addText(const String& text, int x, int y)
    {
        m_elements.push_back(new fText(text, x, y));
    }

    void showForm(const String& title) const;
};






/*
    Dev1
*/
class Joystick
{
protected:
    /**
     * @brief HardwareButton Events
     */
    enum EventKey
    {
        KEY_PRESSED_ENTER,
        KEY_PRESSED_EX,
        KEY_PRESSED_A,
        KEY_PRESSED_B,
        KEY_NOT_PRESSED,
    } EVENT_KEY;
    
    /**
     * @brief Joystick Events
     */
    enum EventJoystick
    {
        JOY_ACTIVE_X,
        JOY_NOT_ACTIVE_X,
        JOY_ACTIVE_Y,
        JOY_NOT_ACTIVE_Y,
    } EVENT_JOYSTICK;
private:
    /* The sticks are in the middle position. */
    int DEF_RES_Y0{1840}, DEF_RES_Y1{1840}, DEF_RES_X0{1840}, DEF_RES_X1{1840};
    /* Correction on the y0-axis. */
    const int8_t CORR_Y0{100}, CORR_Y1{100}, CORR_X0{100}, CORR_X1{100};
    /* Initial setting of coordinates. */
    int COOR_Y0 = 80;  // h/2
    int COOR_Y1 = 80;
    int COOR_X0 = 128; // w/2
    int COOR_X1 = 128;
    /* Reset the counter of objects. */
    int OBJ_Y0{}, OBJ_Y1{}, OBJ_X0{}, OBJ_X1{};
    unsigned long prevTime{};
public:
    /**
     * @brief The variables contain raw data obtained from the joystick.
     */
    int RAW_DATA_Y0{}, RAW_DATA_Y1{}, RAW_DATA_X0{}, RAW_DATA_X1{};
    
    /**
     * @brief Contains the coordinates of the Sticks along the axes.
     * @return Returns integer coordinate values.
     * @warning Do not run more than once.
     */
    int calculatePositionX0();
    int calculatePositionY0();
    int calculatePositionX1();
    int calculatePositionY1();

    /**
     * @brief Variables for storing coordinates from the axes of the Sticks.
     * @warning Before getting the coordinates, run the updatePosition method.
     */
    int posX0{}, posY0{}, posX1{}, posY1{};

    /**
     * @brief Variables for storing Stick indexes.
     * @warning Before getting the coordinates, run the updatePosition method.
     */
    int indexX0{}, indexY0{}, indexX1{}, indexY1{};
    
    /* Generates 1 or 0 if the button is pressed or not. */
    bool pressKeyENTER();
    bool pressKeyEX();
    bool pressKeyA();
    bool pressKeyB();

    /* Updating Stick coordinates. */
    void updatePositionXY();
    /* Updating Stick coordinates. And delay update position. */
    void updatePositionXY(uint delay);
    /* Reset position Sticks */
    void resetPositionXY();

    /* Counts objects by +1, normally 0 */
    int8_t calculateIndexY0();
    int8_t calculateIndexY1();
    int8_t calculateIndexX0();
    int8_t calculateIndexX1();
};

class Graphics
{
protected:
private:
    int image_width{}, image_height{};
public:
    /* Turn on the backlight of the LCD screen. 1 enabled, 0 disabled. */
    bool controlBacklight(bool state);

    /* Initial display setting. Sets Contrast to 0, analog DC at 12, sets port 8 to 1 */
    void initializationSystem();
    /* We send the void-function to the display buffer for output. 
       The void-function will be completed by time-Delay-interval. */
    void render(void (*f)(), int timeDelay);
    /* We send the void-function to the display buffer for output. */
    void render(void (*f)());
    void render(void (*f1)(), void (*f2)());
    /* Clearing the display buffer. */
    void clear();
    /* Data output in x, y coordinates. Size font 5, 6, 7, 8, 10, 12, 13.
       lii-line spacing (10 by default),
       chi-character spacing (6 by default).
       Line break is supported - '\n' */
    void print(int8_t sizeFont, String text, int x, int y, int8_t lii, int8_t chi);
    /* Data output in x, y coordinates. lii-line spacing (10 by default),
       chi-character spacing (6 by default).
       Line break is supported - '\n' */
    void print(String text, int x, int y, int8_t lii, int8_t chi);
    /* Data output in x, y coordinates. 
       Line break is supported - '\n' */
    void print(String text, int x, int y);
    /* Runs a void-function with text-string and output x-y-coordinates parameters.
       The interval-interval controls the output. */
    bool winkPrint(void (*f)(String, int, int), String text, int x, int y, /*delay*/ int interval);
    /* Wait display */
    bool waitDisplay();
};

class Timer
{
protected:
private:
    unsigned long prevTime{};
public:
    /* Starting a void-function on a interval-timer. */
    void timer(void (*f)(void), int interval);
    void timer(int (*f)(void), int interval);
    void stopwatch(void (*f)(void), int interval);
};

class Interface : Joystick
{
private:
    uint8_t xBorder{};
    uint8_t yBorder{};
public:
    /* Output of a message to the user. Define the text-text and duration-duration.
       Line break is supported - '\n'. */
    void message(String text, int duration);
    void popUpMessage(String label, String text, uint tDelay);
    void popUpMessage(String label1, String label2, String text, uint tDelay);
    void popUpMessage(String label, String text);
    bool dialogueMessage(String label, String text);
    bool dialogueMessage(String label, String text, void (*f1)(), void (*f2)());
};

class Button : Joystick
{
private:
    int xCursor, yCursor;
public:
    /* The button starts the void-function, define the button text-text and output x-y-coordinates.
       xCursor-yCursor-coordinates of interaction with the cursor. */
    bool button(String text, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
    /* Return boolean state */
    bool button(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor);
    bool button2(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor);
};

class Shortcut : Joystick
{
private:
public:
    /* A shortcut on the desktop to launch the void-function.
       Define an icon-image with a resolution of 32x32 pixels, x-y-coordinates for output,
       xCursor-yCursor-coordinates of interaction with the cursor. */
    bool shortcut(const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
    /* A shortcut on the desktop to launch the void-function.
       Define an icon-image with a resolution of 32x32 pixels + name */
    bool shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
    
    bool shortcutFrame(String name, uint8_t w, uint8_t h, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);

};

class Label : Joystick
{
private:
public:
    /* Label */
    bool label(String text, uint8_t x, uint8_t y, void (*f)(void), uint8_t lii, uint8_t chi, int xCursor, int yCursor);
    bool label(String text, String description, uint8_t x, uint8_t y, void (*f)(void), uint8_t lii, uint8_t chi, int xCursor, int yCursor);
};

class TextBox
{
private:
public:
    /* Properties Text-box */
    enum objectLocation {left, middle, right};
    enum objectBoundary {noBorder, oneLine, twoLine, shadow, shadowNoFrame};
    /**/
    //String textObject;
    /* Text-box */
    void textBox(String str, objectLocation location, objectBoundary boundary, short charH, short charW, int x, int y);
    void textBox(String str, objectBoundary boundary, int sizeH, int sizeW, short charH, short charW, int x, int y);
};

class Form0
{
private:
    short outerBoundaryForm {20};
    short innerBoundaryForm {5};
public:
    enum objectLocationForm {together, itself};
    void form(String title, String text, objectLocationForm location);
};

class Cursor
{
private:
public:
    /* Cursor. If the stateCursor status is 1 - visible, if 0 - not visible.
       Determine the coordinates of the Cursor to interact with the selected Stick. */
    bool cursor(bool stateCursor, int xCursor, int yCursor);
};

class PowerSave : Joystick
{
private:
    /* If joystick pressed ot moved - 0, else - 1 */
   //bool isTouched();    
public:
    /* Powersave - light mode */
    void sleepLight(bool state, uint timeUntil);
    /* Powersave - deep mode */
    void sleepDeep(bool state, uint timeUntil);
    /* Powersave - double mode */
    //void powerSaveDeepSleep();
};

class Melody
{
private:
    int8_t BEAT = 120;

    uint songMakeGame[7][2] = {{330,77}, {2637,256}, {415,791}, {523,23}, {494,860}, {1865,105}, {2459,128}};
   
    uint songBang1[2][2] = {{100, 100}, {100, 100}};
    uint songBang2[2][2] = {{300, 100}, {300, 100}};
    uint songBang3[2][2] = {{600, 100}, {600, 100}};
    uint songBang4[2][2] = {{900, 100}, {900, 100}};
    uint songBang5[2][2] = {{1200, 100}, {1200, 100}};

    uint songTone1[1][2] = {100, 100};
    uint songTone2[1][2] = {300, 100};
    uint songTone3[1][2] = {600, 100};
    uint songTone4[1][2] = {900, 100};
    uint songTone5[1][2] = {1200, 100};

    /*uint songOk[][2] = {};
    uint songCancel[][2] = {};
    uint songError[][2] = {};
    uint songClick[][2] = {};*/
public:
    enum listMelody
    {
        None,     // 0
        MakeGame, // 1
        
        Bang1,    // 2
        Bang2,    // 3
        Bang3,    // 4
        Bang4,    // 5
        Bang5,    // 6

        Tone1,    // 7
        Tone2,    // 8
        Tone3,    // 9
        Tone4,    // 10
        Tone5,    // 11

        Ok,       // 12
        Cancel,   // 13
        Error,    // 14
        Click,    // 15
    };

    listMelody lM;

    void songCore();
    void song(listMelody num);

};

class Application
{
    protected:
    private:
    public:

    /* Applic */
    void window(String name, int indexTask, void (*f1)(void), void (*f2)(void));
    void window(String name, int indexTask, void (*f1)(void));
};

class Terminal
{
private:
public:
    /* System terminal */
    //void terminal();
    void terminal2(); //vector only
    //void terminal(void(*f)());
};

class TimeNTP
{
private:
public:

};

class Task
{
private:
public:
    void taskKill(int indexTask);
    void taskRun(int indexTask);
    void task(int indexTask);
};

#endif