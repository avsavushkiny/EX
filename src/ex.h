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

//#include <U8g2lib.h>
#include <stack>

#ifndef EX_H
#define EX_H

/* We let the compiler know that the u8g2 object is defined in another file */
extern const int8_t PIN_BACKLIGHT_LCD;
extern int H_LCD, W_LCD;


/*
    Task-dispatcher
    Dispatcher tasks, vector
    01/2025, Alexander Savushkin
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
    // std::vector<TaskStack> taskStack;     // Task Stack Vector
    std::vector<TaskArguments> tasks;     // Vector of main tasks
    std::vector<TaskArguments> userTasks;
};


/*
    exForm
    Visual eForm Builder
    01-02/2025, Alexander Savushkin
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
    // eButton(const String& label, void (*onClick)(), int x, int y) : m_label(label), m_onClick(onClick), m_x(x), m_y(y) {}
    eButton(const String& label, std::function<void()> func, int x, int y) : m_label(label), m_func(func), m_x(x), m_y(y) {}


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
    bool m_stateButton;
private:
    std::function<void()> m_func; // Обёртка для функции
    String m_label; 
    // void (*m_onClick)(void);
    int xForm, yForm, wForm, hForm;
    // short outerBoundaryForm{20};
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
        this->wForm = w + m_w;
        this->hForm = h + m_h;
    }
private:
    // String m_title;
    int xForm, yForm, wForm, hForm;
    int m_x, m_y, m_w{256}, m_h{160};
};
/* Checkbox */
class eCheckbox : public eElement
{
public:
    // eCheckbox(const String& text, int x, int y) : m_text(text), m_x(x), m_y(y) {}
    eCheckbox(bool checked, const String& text, int x, int y) : m_checked(checked), m_text(text), m_x(x), m_y(y) {}

    bool isChecked() const
    {
        return m_checked;
    }

    void setChecked(bool checked)
    {
        m_checked = checked;
    }

    void setText(const String& new_text)
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
    bool m_checked;
    String m_text;
    int xForm, yForm, wForm, hForm;
    int m_x{0}, m_y{0};
};
/* Function */
class eFunction : public eElement
{
public:
    // eFunction(void (*func)()) : m_func(func) {}
    // Конструктор, принимающий std::function
    eFunction(std::function<void()> func) : m_func(func) {}

    void execute() {
        if (m_func) {
            m_func();
        }
    }
    
    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x;
        this->yForm = y;
        this->wForm = w;
        this->hForm = h;
    }
    int xForm, yForm, wForm, hForm;
private:
    // void (*m_func)(void);
    std::function<void()> m_func; // Обёртка для функции
};
/* Picture xbmp */
class ePicture : public eElement
{
public:
    ePicture(const uint8_t *bitmap, int x, int y, int w, int h) : m_bitmap(bitmap), m_x(x), m_y(y), m_w(w), m_h(h) {}

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = 0;
        this->hForm = 0;
    }

private:

    int m_x, m_y, m_w, m_h;
    int xForm, yForm, wForm, hForm;
    const uint8_t *m_bitmap;
};
/* Background */
class eBackground : public eElement
{
public:
    eBackground(const uint8_t *bitmap, int x, int y, int w, int h) : m_bitmap(bitmap), m_x(x), m_y(y), m_w(w), m_h(h) {}

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = 0;
        this->hForm = 0;
    }

private:

    int m_x, m_y, m_w, m_h;
    int xForm, yForm, wForm, hForm;
    const uint8_t *m_bitmap;
};

/* Desktop */
template <typename T>
class eDesktop : public eElement
{
public:
    eDesktop(const std::vector<T>& data) : data_(data) {}
    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x;
        this->yForm = y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    std::vector<T> data_;
    int xForm, yForm, wForm, hForm;
};
/* Graphics */
class eGraphics : public eElement
{
public:
    // eGraphics(void (*func)()) : showFunc(func) {}
    eGraphics(void (*func)(int, int, int, int), int x, int y, int w, int h) : showFunc(func), m_x(x), m_y(y), m_w(w), m_h(h) {}

    void show() override
    {
        showFunc(xForm, yForm, wForm, hForm);
    }

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w + m_w;
        this->hForm = h + m_h;
    }
private:
    int m_x, m_y, m_w, m_h;
    int xForm, yForm, wForm, hForm;
    void (*showFunc)(int, int, int, int);
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
enum EFORMSHOWMODE { FULLSCREEN, MAXIMIZED, NORMAL, FLAT };
enum EFORMBACKGROUND { TRANSPARENT, WHITE, LIGHT_GRAY, DARK_GRAY, BLACK };
class exForm : public eForm
{
public:
    int showForm() override;

    String title = "Title form";
    EFORMSHOWMODE eFormShowMode;
    EFORMBACKGROUND eFormBackground;
private:
    int xForm, yForm;
    short outerBoundaryForm{20};
};

extern std::stack<exForm*> formsStack;

/*
    eFormStack
    Stack of eForm objects
    01/2025, Alexander Savushkin
*/
/* Class for controlling the glass forms */
class exFormStack
{
public:
    // Add the form to the stack
    void push(exForm* form)
    {
        formsStack.push(form);
    }
    // Extract the upper form from the stack
    exForm* pop()
    {
        if (!formsStack.empty())
        {
            exForm* top = formsStack.top();
            formsStack.pop();
            return top;
        }
        return nullptr;
    }
    exForm* top()
    {
        exForm* top = formsStack.top();
        return top;
    }
    // The number of forms in the stack
    size_t size() const
    {
        return formsStack.size();
    }
    // Check, is it empty?
    bool empty() const
    {
        return formsStack.empty();
    }
    // обновляем форму один раз
    void refreshForm()
    {
        if (!formsStack.empty())
        {
            exForm *top = pop(); // Извлекаем верхнюю форму из стека
            push(top);           // Снова добавляем эту форму в стек
        }
    }
    // обновляем форму многократно
    bool updateForm(unsigned int timeUpdate)
    {
        unsigned long currTime = millis();
        if (currTime - prevTime >= timeUpdate)
        {
            prevTime = currTime;

            refreshForm();

            return 1;
        }
        return 0;
    }

private:
    unsigned long prevTime{};
};


/*
    PopUp form
    [02/2025, Alexander Savushkin]
*/
class InstantMessage
{
public:
    InstantMessage(String text, unsigned int tDelay) : m_text(text), m_delay(tDelay) {}

    void show();

    void drawDotGrid(int interval);

private:
    // BorderStyle m_borderMessage;
    String m_label, m_text;
    unsigned int m_delay;
    int xForm{0}, yForm{0}, m_sizeW{256}, m_sizeH{160};
};


/*
    System task
    [02/2025, Alexander Savushkin]
*/
// Абстрактный класс системных элементов
class eSystemElement
{
public:
    virtual void execute() = 0;      // Чисто виртуальный метод
    virtual ~eSystemElement() = default; // Виртуальный деструктор
};
/* Управление подстветкой дисплея */
class eBacklight : public eSystemElement
{
public:
    eBacklight(bool stateLight) : m_stateLight(stateLight) {}

    // Устанавливаем подсветку дисплея
    void execute() override;

    // Устанавливаем новое значение подсветки дисплея
    void setBacklight(const bool newState)
    {
        m_stateLight = newState;
    }
    // Получаем значение статуса подсветки дисплея
    bool getBacklight() const
    {
        return m_stateLight;
    }

private:
    bool m_stateLight;
};
/* Управление контрастностью дисплея */
class eDisplayContrast : public eSystemElement
{
public:
    eDisplayContrast(int value) : m_valueContrast(value) {}

    void execute() override;

    void setDisplayContrast(int newValue)
    {
        m_valueContrast = newValue;
    }

    int getDisplayContrast()
    {
        return m_valueContrast;
    }
private:
    int m_valueContrast;
};
/* Управление режимами энергосбережения*/
class ePowerSave : public eSystemElement
{
public:
    ePowerSave(bool statePowerSave) : m_statePowerSave(statePowerSave) {}

    bool isTouched();

    void execute() override;

private:
    bool m_statePowerSave;
};
/* Управление курсором */
class eCursor : public eSystemElement
{
public:
    eCursor(bool stateCursor) : m_stateCursor(stateCursor) {}

    void execute() override;

private:
    bool m_stateCursor;
};
/* Управление портом данных */
class eDataPort : public eSystemElement
{
public:
    eDataPort(bool stateDataPort, int port) : m_stateDataPort(stateDataPort), m_port(port) {}

    void execute() override;

private:
    bool m_stateDataPort;
    int m_port;
};



/*
    Systems
    [02/2025, Alexander Savushkin]
*/
class Systems
{
public:
    // Конструктор, инициализирующий все подсистемы
    Systems(bool backlightState, int valueContrast, bool powerSaveState, bool cursorState, bool dataPortState, int port)
        : backlight(backlightState), displayContrast(valueContrast), powerSave(powerSaveState), cursor(cursorState), dataPort(dataPortState, port) {}

    // Методы для управления подсветкой
    bool STATEBACKLIGHT;
    void setBacklight(bool newState)
    {
        backlight.setBacklight(newState);
    }
    bool getBacklight() const
    {
        return backlight.getBacklight();
    }

    // Методы установки контрастности дисплея
    int VALUECONTRAST{240}; // 143 old value
    void setDisplayContrast(int newValue)
    {
        displayContrast.setDisplayContrast(newValue);
    }
    int getDisplayContrast()
    {
        return displayContrast.getDisplayContrast();
    }

    // Методы для выполнения задач всех подсистем
    void executeAllSystemElements()
    {
        backlight.execute();
        displayContrast.execute();
        powerSave.execute();
        cursor.execute();
        dataPort.execute();
    }

private:
    eBacklight backlight; // Управление подсветкой
    eDisplayContrast displayContrast; // Установка контрастности дисплея
    ePowerSave powerSave; // Управление режимами энергосбережения
    eCursor cursor;       // Управление курсором
    eDataPort dataPort;   // Управление портом данных
};


/*
    Relay
    Changing the state of a variable
    02/2025, Alexander Savushkin
*/
class Relay
{
private:
    bool m_value;

public:
    Relay(bool value)
    {
        m_value = value;
    }

    void setValue(bool value)
    {
        m_value = value;
    }

    bool getValue() const
    {
        return m_value;
    }
};


/*
    Text-buffer
    Storing a line of text on the stack
    01/2025, Alexander Savushkin
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
    /* Data output in x, y coordinates. lii-line spacing (10 by default),
       chi-character spacing (6 by default).
       Line break is supported - '\n' */
    void print(String text, int x, int y, int8_t lii, int8_t chi);
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
    bool timer(int interval);
    void timer(int (*f)(void), int interval);
};

class Interface : Joystick
{
private:
    uint8_t xBorder{};
    uint8_t yBorder{};
public:
};

class Button : Joystick
{
private:
    int xCursor, yCursor;
public:
    bool button2(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor);
};

class Shortcut : Joystick
{
private:
public:
    /* A shortcut on the desktop to launch the void-function.
       Define an icon-image with a resolution of 32x32 pixels + name */
    bool shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
};

class Label : Joystick
{
private:
public:
};

class TextBox
{
private:
public:
    /* Properties Text-box */
    enum objectLocation {left, middle, right};
    enum objectBoundary {noBorder, oneLine, twoLine, shadow, shadowNoFrame};

    void textBox(String str, int sizeH, int sizeW, short charH, short charW, int x, int y);
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
};

class Melody
{
private:
public:
};

class Terminal
{
private:
public:
};

#endif