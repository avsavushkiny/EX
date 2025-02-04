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
#include <stack>

#ifndef EX_H
#define EX_H

/* We let the compiler know that the u8g2 object is defined in another file */
extern U8G2_ST75256_JLX256160_F_4W_HW_SPI u8g2;
extern const uint8_t gears_bits[];
extern int H_LCD, W_LCD;

extern void _clearCommandTerminal();

/*
    Task-dispatcher
    Dispatcher tasks, vector
    [01/2025, Alexander Savushkin]
*/
/* Task Settings */
enum TaskType
{
    SYSTEM,
    DESKTOP,
    USER
};
/* Task Settings */
struct TaskArguments
{
    String name;           // Short task name
    void (*f)(void);       // Pointer to task function
    const uint8_t *bitMap; // Pointer to xbmp image
    TaskType type;         // Task type, 0 - system, 1 - desktop, 3 - user
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
    // Make the Task inactive
    bool removeTaskIndex(const int index);
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
    eForm
    Visual eForm Builder
    [01/2025, Alexander Savushkin]
*/
enum BorderStyle {noBorder, oneLine, twoLine, shadow, shadowNoFrame};
/* Basic interface for all form elements */
class eElement
{
public:
    virtual ~eElement(){}
    /* чистая функция для вывода элемента */
    virtual void show() = 0;

    /* чистая функция для установки позиции элемента */
    virtual void setPosition(int x, int y, int w, int h) = 0;
    int m_x{0}, m_y{0}, m_w{0}, m_h{0};
};
/* Button */
class eButton : public eElement
{
public:    
    eButton(const String& label, void (*onClick)(), int x, int y) : m_label(label), m_onClick(onClick), m_x(x), m_y(y) {}

    void setLabel(const String &new_label)
    {
        m_label = new_label;
    }

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    String m_label; 
    void (*m_onClick)(void);
    int xForm, yForm, wForm, hForm;
    short outerBoundaryForm{20};
    int m_x{0}, m_y{0};
};
/* Text multiline */
class eText : public eElement
{
public:
    eText(const String& text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

    void setText(const String &new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    String m_text;
    short const highChar{10};
    int xForm, yForm, wForm, hForm;
    short outerBoundaryForm{20};
    int m_x{0}, m_y{0};
};
/* Textbox */
class eTextBox : public eElement
{  
public:
    eTextBox(const String& text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y) : m_text(text), m_borderStyle(borderStyle), m_sizeW(sizeW), m_sizeH(sizeH), m_x(x), m_y(y) {}

    void setText(const String &new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    BorderStyle m_borderStyle;
    String m_text;
    int xForm, yForm, wForm, hForm;
    short outerBoundaryForm{20};
    int m_x{0}, m_y{0};
    int m_sizeW, m_sizeH;
};
/* Label */
class eLabel : public eElement
{
public:
    eLabel(const String& text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

    void setText(const String &new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    String m_text;
    int xForm, yForm, wForm, hForm;
    int m_x{0}, m_y{0};
};
/* Label to link */
class eLinkLabel : public eElement
{
public:
    eLinkLabel(const String& text, void (*onClick)(), int x, int y) : m_text(text), m_onClick(onClick), m_x(x), m_y(y) {}

    void setText(const String &new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    void (*m_onClick)(void);
    String m_text;
    int xForm, yForm, wForm, hForm;
    int m_x{0}, m_y{0};
    int m_sizeW, m_sizeH;
};
/* Horizontal line */
class eLine : public eElement
{
public:
    eLine(int x, int y) : m_x(x), m_y(y) {}
    
    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }
private:
    // String m_title;
    int xForm, yForm, wForm, hForm;
    int m_x{0}, m_y{0}, m_w{0};
};


// Класс VirtualKeyboard, наследующийся от eElement
class eVirtualKeyboard : public eElement
{
public:
    eVirtualKeyboard(int x, int y) : m_x(x), m_y(y) {}

    void show() override;
    // {
    //     // Отображаем клавиатуру
    //     // Все что ввели через кнопку бросаем в m_input
    //     // Отображаем полученный ввод
    // }

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

    void setInput(const String &new_text)
    {
        m_input = new_text;
    }

    String getInput()
    {
        return m_input;
    }

private:
    String m_input;
    int xForm, yForm, wForm, hForm;
    int m_x, m_y;
};

// Класс InputBox, наследующийся от eElement
class eInputBox : public eElement
{
public:
    eInputBox(int x, int y) : m_x(x), m_y(y) {}

    void show() override
    {
        // Вызываем виртуальную клавиатуру для получения ввода
        // VirtualKeyboard vk(m_x, m_y);
        // vk.show();
        // m_input = vk.getInput();
        // Отображаем полученный ввод
    }

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    String m_input;
    int xForm, yForm, wForm, hForm;
    int m_x, m_y;
};

/* [!] List box */
class eListBox : public eElement
{
public:
    template<typename T>
    eListBox(std::vector<T>& t, int x, int y) : m_x(x), m_y(y) {}

    void show() override
    {
    }

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    int xForm, yForm, wForm, hForm, m_x, m_y;
};


/* Desktop */
class eDesktop : public eElement
{
public:
    eDesktop() {}
    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x;
        this->yForm = y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    int xForm, yForm, wForm, hForm;
};
/* Graphics */
class eGraphics : public eElement
{
public:
    eGraphics(void (*func)()) : showFunc(func) {}

    void show() override
    {
        showFunc();
    }
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x;
        this->yForm = y;
        this->wForm = w;
        this->hForm = h;
    }
private:
    int xForm, yForm, wForm, hForm;
    void (*showFunc)();
};
/* Abstract base class eForm */
class eForm
{
public:
    virtual ~eForm()
    {
        for (auto element : elements)
        {
            delete element;
        }
    }

    void addElement(eElement *element)
    {
        elements.push_back(element);
    }

    virtual int showForm() = 0;

protected:
    std::vector<eElement*> elements;
};
/* Implementation of a concrete class exForm */
enum EFORMSHOWMODE { FULLSCREEN, MAXIMIZED, NORMAL };
class exForm : public eForm
{
public:
    int showForm() override;

    String title = "Title form";
    EFORMSHOWMODE eFormShowMode;
private:
    int xForm, yForm;
    short outerBoundaryForm{20};
};


/*
    eFormStack
    Stack of eForm objects
    [01/2025, Alexander Savushkin] 270125_2336
*/
/* Class for controlling the glass forms */
class exFormsStack
{
public:
    // Add the form to the stack
    void push(exForm* form)
    {
        stack.push(form);
    }

    // Extract the upper form from the stack
    exForm* pop()
    {
        if (!stack.empty())
        {
            exForm* top = stack.top();
            stack.pop();
            return top;
        }
        return nullptr;
    }

    // The number of forms in the stack
    size_t size() const
    {
        return stack.size();
    }

    // Check, is it empty?
    bool empty() const
    {
        return stack.empty();
    }

    /* example of use

    std::stack<exForm*> formsStack;

    exForm* form1 = new exForm(); // Создаем несколько экземпляров классов exForm
    exForm* form2 = new exForm();
    exForm* form3 = new exForm();

    formsStack.push(form1); // Добавляем формы в стек
    formsStack.push(form2);
    formsStack.push(form3);

    std::cout << "Количество форм в стеке: " << formsStack.size() << std::endl;

    while (!formsStack.empty()) // Извлекаем формы из стека
    {
        exForm* currentForm = formsStack.top();
        formsStack.pop();

        int result = currentForm->showForm("Форма"); // Вызываем метод showForm и получаем результат

        if (result == 1) {
            delete currentForm; // Освобождаем память, если результат равен 1
        }
    }

    */

private:
    // Stack of forms
    std::stack<exForm*> stack;
};








/*
    Text-buffer
    Storing a line of text on the stack
    [01/2025, Alexander Savushkin]
*/
/* Base text buffer class */
class TextBuffer
{
private:
    std::stack<String> buffer;

public:
    /* put the line on the top of the stack */
    void add(const String& text)
    {
        buffer.push(text);
    }

    /* extract the top line, display and delete it */
    String get()
    {
        // if the stack is empty, then return an empty line
        if (buffer.empty()) return "";
        // pop the top element from the stack
        String result = buffer.top();
        // remove the top element from the stack
        buffer.pop();
        // return the line
        return result;
    }
};
/*
    Trigger
    Trigger
    [01/2025, Alexander Savushkin] 270125_0116
*/
class Trigger {
private:
    bool m_value;

public:
    Trigger(bool value1, bool value2)
    {
        m_value = value1 && value2;
    }

    bool getValue() const
    {
        return m_value;
    }
};
/*
    Action-menu
    Additional Actions Menu
    [01/2025, Alexander Savushkin] 270125_0116
*/
class FormActionMenuElements
{
protected:
    virtual void show() const = 0;
};

class FormActionMenuTextBox : FormActionMenuElements
{
private:
    String fam_text;
public:
    FormActionMenuTextBox(const String& text) : fam_text(text) {}

    void show() const override {};
};

class FormActionMenu
{
private:
    std::vector<FormActionMenuElements*> fam_elements;
public:
    ~FormActionMenu()
    {
        for (FormActionMenuElements* elements : fam_elements)
        {
            delete elements;
        }
    }

    void showActionMenu(const String& title) const {};
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
private:
    uint8_t _FPS = 0; uint8_t _fpsCounter = 0; long int _fpsTime = millis();
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
    /* Рассчет кадров/сек */
    int calculateFPS();
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
    /* ? */
    bool label(String text, uint8_t x, uint8_t y, void (*f)(void), uint8_t lii, uint8_t chi, int xCursor, int yCursor);
    /* 010225 */
    void label(String text, void (*f)(void), uint8_t x, uint8_t y);
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
    void textBox(String str, int sizeH, int sizeW, short charH, short charW, int x, int y);
};

class Cursor
{
private:
public:
    /* Cursor. If the stateCursor status is 1 - visible, if 0 - not visible.
       Determine the coordinates of the Cursor to interact with the selected Stick. */
    bool cursor(bool stateCursor, int xCursor, int yCursor);
    bool cursor(bool stateCursor);
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

class Terminal
{
private:
public:
    /* System terminal */
    //void terminal();
    void terminal2(); //vector only
    //void terminal(void(*f)());
};

#endif