#pragma once

#include <Arduino.h>
#include <stack>
#include "ggl.h"
#include "input.h"
#include "graphics.h"
#include "taskDispatcher.h"
#include "ui.h"

extern GGL _GGL;
extern Joystick _JOY;
extern Graphics _GRF;

enum BorderStyle
{
    noBorder,
    oneLine,
    twoLine,
    shadow,
    shadowNoFrame
};
/* Basic interface for all form elements */
class eElement
{
public:
    virtual ~eElement() {}
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
    eButton(const String &label, std::function<void()> func, int x, int y) : m_label(label), m_func(func), m_x(x), m_y(y) {}

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
    eText(const String &text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

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
    eTextBox(const String &text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y) : m_text(text), m_borderStyle(borderStyle), m_sizeW(sizeW), m_sizeH(sizeH), m_x(x), m_y(y) {}

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
    eLabel(const String &text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

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
    eLinkLabel(const String &text, void (*onClick)(), int x, int y) : m_text(text), m_onClick(onClick), m_x(x), m_y(y) {}

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
    eCheckbox(bool checked, const String &text, int x, int y) : m_checked(checked), m_text(text), m_x(x), m_y(y) {}

    bool isChecked() const
    {
        return m_checked;
    }

    void setChecked(bool checked)
    {
        m_checked = checked;
    }

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

    void execute()
    {
        if (m_func)
        {
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
    eDesktop(const std::vector<T> &data) : data_(data) {}
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
    virtual int showForm() = 0;
    
    void addElement(eElement *element)
    {
        elements.push_back(element);
    }

    virtual ~eForm()
    {
        for (auto element : elements)
        {
            delete element;
        }
    }

protected:
    std::vector<eElement *> elements;
};

/* Implementation of a concrete class exForm */
enum EFORMSHOWMODE
{
    FULLSCREEN,
    MAXIMIZED,
    NORMAL,
    FLAT
};
enum EFORMBACKGROUND
{
    TRANSPARENT,
    WHITE,
    LIGHT_GRAY,
    DARK_GRAY,
    BLACK
};

/* Base class exForm */
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

extern std::stack<exForm *> formsStack;

/* Class for controlling the glass forms */
class exFormStack
{
public:
    // Add the form to the stack
    void push(exForm *form)
    {
        formsStack.push(form);
    }
    // Extract the upper form from the stack
    exForm *pop()
    {
        if (!formsStack.empty())
        {
            exForm *top = formsStack.top();
            formsStack.pop();
            return top;
        }
        return nullptr;
    }
    //
    exForm *top()
    {
        exForm *top = formsStack.top();
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
