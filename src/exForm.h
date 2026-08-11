#pragma once

#include <Arduino.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <functional>
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
    virtual void show(const std::vector<eElement *> &allElements) { show(); } // Для совместимости

    /* чистая функция для установки позиции элемента */
    virtual void setPosition(int x, int y, int w, int h) = 0;

    /* Методы для работы с Z-индексом */
    virtual void setZIndex(int zIndex) { m_zIndex = zIndex; }
    virtual int getZIndex() const { return m_zIndex; }

    /* Проверка, находится ли курсор над элементом */
    virtual bool isPointOverElement(int x, int y) const
    {
        return (x >= m_x && x <= m_x + m_w && y >= m_y && y <= m_y + m_h);
    }

    /* Проверка перекрытия двух элементов */
    static bool isRectOverlap(const eElement *a, const eElement *b)
    {
        if (!a || !b)
            return false;
        return !(a->m_x + a->m_w <= b->m_x ||
                 b->m_x + b->m_w <= a->m_x ||
                 a->m_y + a->m_h <= b->m_y ||
                 b->m_y + b->m_h <= a->m_y);
    }

    /* Проверка, перекрыт ли текущий элемент более верхними */
    bool isOverlappedByHigher(const std::vector<eElement *> &allElements)
    {
        for (auto *other : allElements)
        {
            if (other == this)
                continue;
            if (other->getZIndex() > this->getZIndex() &&
                isRectOverlap(this, other))
            {
                return true;
            }
        }
        return false;
    }

    /* Проверка, перекрыт ли текущий элемент более верхними в точке курсора */
    bool isOverlappedAtPoint(const std::vector<eElement *> &allElements, int x, int y)
    {
        for (auto *other : allElements)
        {
            if (other == this)
                continue;
            if (other->getZIndex() > this->getZIndex() &&
                other->isPointOverElement(x, y))
            {
                return true;
            }
        }
        return false;
    }

    int m_x{0}, m_y{0}, m_w{0}, m_h{0};
    int m_zIndex{0}; // Чем больше число, тем выше элемент
    bool m_enabled{true};
    bool m_visible{true};
};

/* Button */
class eButton : public eElement
{
public:
    eButton(const String &label, std::function<void()> func, int x, int y)
        : m_label(label), m_func(func), m_x(x), m_y(y) {}

    void setLabel(const String &new_label)
    {
        m_label = new_label;
    }

    void show() override;
    void show(const std::vector<eElement *> &allElements) override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

    bool m_stateButton{false};

private:
    std::function<void()> m_func;
    String m_label;
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
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    String m_text;
    int m_x{0}, m_y{0};
};

/* Textbox */
class eTextBox : public eElement
{
public:
    eTextBox(const String &text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y)
        : m_text(text), m_borderStyle(borderStyle), m_sizeW(sizeW), m_sizeH(sizeH), m_x(x), m_y(y) {}

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
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    BorderStyle m_borderStyle;
    String m_text;
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
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    String m_text;
    int m_x{0}, m_y{0};
};

/* Label to link */
class eLinkLabel : public eElement
{
public:
    eLinkLabel(const String &text, void (*onClick)(), int x, int y)
        : m_text(text), m_onClick(onClick), m_x(x), m_y(y) {}

    void setText(const String &new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() override;
    void show(const std::vector<eElement *> &allElements) override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    void (*m_onClick)(void);
    String m_text;
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
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w + this->m_w;
        this->m_h = h + this->m_h;
    }

private:
    int m_x, m_y, m_w{256}, m_h{160};
};

/* Checkbox */
class eCheckbox : public eElement
{
public:
    eCheckbox(bool checked, const String &text, int x, int y)
        : m_checked(checked), m_text(text), m_x(x), m_y(y) {}

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
    void show(const std::vector<eElement *> &allElements) override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    bool m_checked;
    String m_text;
    int m_x{0}, m_y{0};
};

/* Function */
class eFunction : public eElement
{
public:
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
        this->m_x = x;
        this->m_y = y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    std::function<void()> m_func;
};

/* Picture xbmp */
class ePicture : public eElement
{
public:
    ePicture(const uint8_t *bitmap, int x, int y, int w, int h)
        : m_bitmap(bitmap), m_x(x), m_y(y), m_w(w), m_h(h) {}

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = 0;
        this->m_h = 0;
    }

private:
    int m_x, m_y, m_w, m_h;
    const uint8_t *m_bitmap;
};

/* Background */
class eBackground : public eElement
{
public:
    eBackground(const uint8_t *bitmap, int x, int y, int w, int h)
        : m_bitmap(bitmap), m_x(x), m_y(y), m_w(w), m_h(h) {}

    void show() override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = 0;
        this->m_h = 0;
    }

private:
    int m_x, m_y, m_w, m_h;
    const uint8_t *m_bitmap;
};

/* Keyboard */
class eKeyboard : public eElement
{
public:
    std::vector<String> row1, row2, row3;

    eKeyboard(std::function<void(char)> onCharInput, int x, int y, int keyW = 18, int keyH = 14)
        : m_onCharInput(onCharInput), m_x(x), m_y(y), m_keyW(keyW), m_keyH(keyH),
          m_lastKeyPressTime(0), m_keyRepeatDelay(200), m_capsLock(false)
    {
        row1 = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"};
        row2 = {"A", "S", "D", "F", "G", "H", "J", "K", "L", "Z"};
        row3 = {"X", "C", "V", " ", "B", "N", "M", "BS", "CL"};
    }

    void show() override;
    void show(const std::vector<eElement *> &allElements) override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;
    }

    String getText() const
    {
        return m_inputText;
    }

    void clearText()
    {
        m_inputText = "";
    }

    void setText(const String &text)
    {
        m_inputText = text;
    }

    bool isActive() const
    {
        return m_active;
    }

    void setActive(bool active)
    {
        m_active = active;
    }

    void setKeyRepeatDelay(unsigned long delayMs)
    {
        m_keyRepeatDelay = delayMs;
    }

    bool isCapsLock() const
    {
        return m_capsLock;
    }

    void setCapsLock(bool enabled)
    {
        m_capsLock = enabled;
    }

    void getKeyboardSize(int &width, int &height, int keyW, int keyH) const
    {
        int keySpacing = 2;
        int maxRowSize = max(row1.size(), max(row2.size(), row3.size()));
        width = maxRowSize * (keyW + keySpacing) + keySpacing;
        height = 3 * (keyH + keySpacing) + keySpacing;
    }

private:
    std::function<void(char)> m_onCharInput;
    String m_inputText;
    bool m_active{true};
    bool m_capsLock;

    int m_x, m_y;
    int m_keyW, m_keyH;

    unsigned long m_lastKeyPressTime;
    unsigned long m_keyRepeatDelay;

    void drawKey(int x, int y, int w, int h, const String &label, bool highlighted);
    bool isKeyPressed(int x, int y, int w, int h);
};

/* Text Input with Keyboard */
class eTextInput : public eElement
{
public:
    eTextInput(const String &label, int x, int y, int width, int height,
               std::function<void(const String &)> onTextChanged = nullptr)
        : m_label(label), m_x(x), m_y(y), m_width(width), m_height(height),
          m_onTextChanged(onTextChanged), m_isEditing(false), m_lastToggleTime(0)
    {
        m_keyboard = new eKeyboard(
            [this](char ch)
            { this->onCharInput(ch); },
            x, y + height + 5, 18, 14);
        m_keyboard->setZIndex(100); // Клавиатура всегда сверху
    }

    ~eTextInput()
    {
        delete m_keyboard;
    }

    void show() override;
    void show(const std::vector<eElement *> &allElements) override;

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w;
        this->m_h = h;

        if (m_keyboard)
        {
            m_keyboard->setPosition(x, y + m_height + 5, w, 0);
        }
    }

    void setText(const String &text)
    {
        m_text = text;
        if (m_keyboard)
        {
            m_keyboard->setText(text);
        }
    }

    String getText() const
    {
        return m_text;
    }

    void clearText()
    {
        m_text = "";
        if (m_keyboard)
        {
            m_keyboard->clearText();
        }
    }

    void setEditing(bool editing)
    {
        m_isEditing = editing;
        if (m_keyboard)
        {
            m_keyboard->setActive(editing);
        }
        m_lastToggleTime = millis();
    }

    bool isEditing() const
    {
        return m_isEditing;
    }

    void setKeyRepeatDelay(unsigned long delayMs)
    {
        if (m_keyboard)
        {
            m_keyboard->setKeyRepeatDelay(delayMs);
        }
    }

    bool isCapsLock() const
    {
        return m_keyboard ? m_keyboard->isCapsLock() : false;
    }

    void setCapsLock(bool enabled)
    {
        if (m_keyboard)
        {
            m_keyboard->setCapsLock(enabled);
        }
    }

private:
    void onCharInput(char ch)
    {
        if (ch == '\b')
        {
            if (m_text.length() > 0)
            {
                m_text.remove(m_text.length() - 1);
            }
        }
        else
        {
            m_text += ch;
        }

        if (m_keyboard)
        {
            m_keyboard->setText(m_text);
        }

        if (m_onTextChanged)
        {
            m_onTextChanged(m_text);
        }
    }

    bool isPointInRect(int x, int y, int rx, int ry, int rw, int rh) const
    {
        return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh);
    }

    String m_label;
    String m_text;
    int m_x, m_y;
    int m_width, m_height;
    bool m_isEditing;
    eKeyboard *m_keyboard;
    std::function<void(const String &)> m_onTextChanged;
    unsigned long m_lastToggleTime;
    static const unsigned long TOGGLE_COOLDOWN = 250;
};

/* Desktop */
template <typename T>
class eDesktop : public eElement
{
public:
    eDesktop(const std::vector<T> &data) : data_(data) {}

    void show() override
    {
        uint8_t border{4};
        uint8_t xx{border};
        uint8_t yy{16};

        Shortcut _shortcutDesktop;
        uint8_t countTask{1};

        for (TaskArguments &t : data_)
        {
            if ((t.activ == false) && (t.bitMap != NULL) && (t.type == DESKTOP))
            {
                _shortcutDesktop.shortcut(t.name, t.bitMap, xx, yy, t.f, _JOY.posX0, _JOY.posY0);
                countTask++;
                xx += (32 + border);

                if (countTask > 7)
                {
                    xx = 4;
                    yy += (32 + border + 16);
                    countTask = 0;
                }
            }
        }
    }

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x;
        this->m_y = y;
        this->m_w = w;
        this->m_h = h;
    }

private:
    std::vector<T> data_;
};

/* Graphics */
class eGraphics : public eElement
{
public:
    eGraphics(void (*func)(int, int, int, int), int x, int y, int w, int h)
        : showFunc(func), m_x(x), m_y(y), m_w(w), m_h(h) {}

    void show() override
    {
        showFunc(m_x, m_y, m_w, m_h);
    }

    void setPosition(int x, int y, int w, int h) override
    {
        this->m_x = x + this->m_x;
        this->m_y = y + this->m_y;
        this->m_w = w + this->m_w;
        this->m_h = h + this->m_h;
    }

private:
    int m_x, m_y, m_w, m_h;
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
        sortElementsByZIndex();
    }

    void removeElement(eElement *element)
    {
        auto it = std::find(elements.begin(), elements.end(), element);
        if (it != elements.end())
        {
            elements.erase(it);
        }
    }

    void sortElementsByZIndex()
    {
        std::sort(elements.begin(), elements.end(),
                  [](eElement *a, eElement *b)
                  {
                      return a->getZIndex() < b->getZIndex();
                  });
    }

    // Получить все элементы формы
    const std::vector<eElement *> &getElements() const
    {
        return elements;
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
    int m_x, m_y;
    short outerBoundaryForm{20};
};

extern std::stack<exForm *> formsStack;

/* Class for controlling the glass forms */
class exFormStack
{
public:
    void push(exForm *form)
    {
        formsStack.push(form);
    }

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

    exForm *top()
    {
        if (!formsStack.empty())
        {
            return formsStack.top();
        }
        return nullptr;
    }

    size_t size() const
    {
        return formsStack.size();
    }

    bool empty() const
    {
        return formsStack.empty();
    }

    void refreshForm()
    {
        if (!formsStack.empty())
        {
            exForm *top = pop();
            push(top);
        }
    }

    bool updateForm(unsigned int timeUpdate)
    {
        unsigned long currTime = millis();
        if (currTime - prevTime >= timeUpdate)
        {
            prevTime = currTime;
            refreshForm();
            return true;
        }
        return false;
    }

private:
    unsigned long prevTime{};
};