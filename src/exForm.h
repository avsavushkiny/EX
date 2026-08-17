#pragma once

#include <Arduino.h>
#include <stack>
#include <vector>
#include <functional>
#include <algorithm>
#include "ggl.h"
#include "input.h"
#include "graphics.h"
#include "taskDispatcher.h"
#include "ui.h"
#include "simpleEvents.h"
#include <WiFi.h>
#include <HTTPClient.h>

extern GGL _GGL;
extern Joystick _JOY;
extern Graphics _GRF;
extern SimpleEventSystem Events;

enum BorderStyle
{
    noBorder,
    oneLine,
    twoLine,
    shadow,
    shadowNoFrame
};

/* Типы элементов для определения приоритета отрисовки */
enum ElementType
{
    ELEM_TYPE_BACKGROUND = 0,
    ELEM_TYPE_NORMAL = 1,
    ELEM_TYPE_TEXT_INPUT = 2,
    ELEM_TYPE_KEYBOARD = 3,
    ELEM_TYPE_POPUP = 4
};

/* Базовый класс для всех элементов формы */
class eElement
{
public:
    virtual ~eElement() {}
    virtual void show() = 0;
    virtual void setPosition(int x, int y, int w, int h) = 0;
    virtual bool isActiveElement() const { return false; }
    virtual void setActive(bool active) {}
    virtual bool isActive() const { return true; }
    virtual bool isInEditMode() const { return false; }
    virtual bool isEditing() const { return false; }
    virtual void setEditing(bool editing) {}
    virtual void deactivate() {}
    virtual ElementType getElementType() const { return ELEM_TYPE_NORMAL; }
    virtual int getZOrder() const { return m_zOrder; }
    virtual void setZOrder(int order) { m_zOrder = order; }
    int m_x{0}, m_y{0}, m_w{0}, m_h{0};
    int m_zOrder{0};
};

/* Базовый класс для интерактивных элементов с поддержкой активности */
class eActiveElement : public eElement
{
public:
    virtual ~eActiveElement() {}

    void setActive(bool active) { m_isActive = active; }
    bool isActive() const override { return m_isActive; }
    bool isActiveElement() const override { return true; }
    virtual bool canAcceptInput() const { return m_isActive; }
    virtual void deactivate() override { m_isActive = false; }

protected:
    bool m_isActive{true};
};

/* Глобальный указатель на активный элемент */
extern eElement *g_activeElement;

/* Класс для управления порядком отрисовки элементов */
class ElementZOrderManager
{
public:
    static void sortElements(std::vector<eElement *> &elements)
    {
        std::sort(elements.begin(), elements.end(),
                  [](eElement *a, eElement *b)
                  {
                      return a->getZOrder() < b->getZOrder();
                  });
    }

    static void bringToFront(eElement *element)
    {
        if (element != nullptr)
        {
            element->setZOrder(getMaxZOrder() + 1);
        }
    }

    static void sendToBack(eElement *element)
    {
        if (element != nullptr)
        {
            element->setZOrder(0);
        }
    }

    static int getMaxZOrder()
    {
        return 1000;
    }
};

/* Button */
class eButton : public eActiveElement
{
public:
    eButton(const String &label, std::function<void()> func, int x, int y)
        : m_label(label), m_func(func), m_x(x), m_y(y)
    {
        m_zOrder = 10;
    }

    void setLabel(const String &new_label) { m_label = new_label; }
    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }
    bool m_stateButton{false};

private:
    std::function<void()> m_func;
    String m_label;
    int xForm, yForm, wForm, hForm;
    int m_x{0}, m_y{0};
};

/* Text multiline */
class eText : public eElement
{
public:
    eText(const String &text, int x, int y) : m_text(text), m_x(x), m_y(y)
    {
        m_zOrder = 5;
    }

    void setText(const String &new_text) { m_text = new_text; }
    String getText() const { return m_text; }
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
class eTextBox : public eActiveElement
{
public:
    eTextBox(const String &text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y)
        : m_text(text), m_borderStyle(borderStyle), m_sizeW(sizeW), m_sizeH(sizeH), m_x(x), m_y(y)
    {
        m_zOrder = 10;
    }

    void setText(const String &new_text) { m_text = new_text; }
    String getText() const { return m_text; }
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

/* eTextScrollBox */
/* Text Scroll Box - для вывода длинного текста с прокруткой */
class eTextScrollBox : public eActiveElement
{
public:
    eTextScrollBox(const String &text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y)
        : m_text(text), m_borderStyle(borderStyle), m_sizeW(sizeW), m_sizeH(sizeH), m_x(x), m_y(y),
          m_scrollOffset(0), m_maxScrollOffset(0), m_isScrollingUp(false), m_isScrollingDown(false)
    {
        m_zOrder = 10;
        calculateMaxScroll();
    }

    void setText(const String &new_text) 
    { 
        m_text = new_text; 
        calculateMaxScroll();
        m_scrollOffset = 0;
    }
    
    String getText() const { return m_text; }
    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    void calculateMaxScroll();
    void drawScrollBar(int scrollX, int scrollY, int scrollH);
    void drawTextWithScroll();
    bool isPointInRect(int px, int py, int rx, int ry, int rw, int rh) const;

    BorderStyle m_borderStyle;
    String m_text;
    int xForm{0}, yForm{0}, wForm{0}, hForm{0};
    int m_x{0}, m_y{0};
    int m_sizeW, m_sizeH;
    
    int m_scrollOffset{0};        // Текущее смещение прокрутки в строках
    int m_maxScrollOffset{0};     // Максимальное смещение
    static const int SCROLL_BAR_WIDTH = 9;
    bool m_isScrollingUp{false};
    bool m_isScrollingDown{false};
    unsigned long m_lastScrollTime{0};
    static const unsigned long SCROLL_DELAY = 150; // Задержка между шагами прокрутки
};

/* Label */
class eLabel : public eElement
{
public:
    eLabel(const String &text, int x, int y) : m_text(text), m_x(x), m_y(y)
    {
        m_zOrder = 5;
    }

    void setText(const String &new_text) { m_text = new_text; }
    String getText() const { return m_text; }
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
class eLinkLabel : public eActiveElement
{
public:
    // Изменяем сигнатуру на std::function
    eLinkLabel(const String &text, std::function<void()> onClick, int x, int y)
        : m_text(text), m_onClick(onClick), m_x(x), m_y(y)
    {
        m_zOrder = 10;
    }

    void setText(const String &new_text) { m_text = new_text; }
    String getText() const { return m_text; }
    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

private:
    std::function<void()> m_onClick;  // Изменяем тип
    String m_text;
    int xForm, yForm, wForm, hForm;
    int m_x{0}, m_y{0};
    int m_sizeW, m_sizeH;
};

/* link */
class eLink : public eActiveElement
{
public:
    eLink(const String &text, void (*onClick)(), int x, int y)
        : m_text(text), m_onClick(onClick), m_x(x), m_y(y)
    {
        m_zOrder = 10;
    }

    void setText(const String &new_text) { m_text = new_text; }
    String getText() const { return m_text; }
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
    eLine(int x, int y) : m_x(x), m_y(y)
    {
        m_zOrder = 3;
    }

    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w + m_w;
        this->hForm = h + m_h;
    }

private:
    int xForm, yForm, wForm, hForm;
    int m_x, m_y, m_w{256}, m_h{160};
};

/* Checkbox */
class eCheckbox : public eActiveElement
{
public:
    eCheckbox(bool checked, const String &text, int x, int y)
        : m_checked(checked), m_text(text), m_x(x), m_y(y)
    {
        m_zOrder = 10;
    }

    bool isChecked() const { return m_checked; }
    void setChecked(bool checked) { m_checked = checked; }
    void setText(const String &new_text) { m_text = new_text; }
    String getText() const { return m_text; }
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
    eFunction(std::function<void()> func) : m_func(func)
    {
        m_zOrder = 1;
    }

    void execute()
    {
        if (m_func)
            m_func();
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
    std::function<void()> m_func;
};

/* Picture xbmp */
class ePicture : public eElement
{
public:
    ePicture(const uint8_t *bitmap, int x, int y, int w, int h)
        : m_bitmap(bitmap), m_x(x), m_y(y), m_w(w), m_h(h)
    {
        m_zOrder = 2;
    }

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
    eBackground(const uint8_t *bitmap, int x, int y, int w, int h)
        : m_bitmap(bitmap), m_x(x), m_y(y), m_w(w), m_h(h)
    {
        m_zOrder = 0;
    }

    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = 0;
        this->hForm = 0;
    }
    ElementType getElementType() const override { return ELEM_TYPE_BACKGROUND; }

private:
    int m_x, m_y, m_w, m_h;
    int xForm, yForm, wForm, hForm;
    const uint8_t *m_bitmap;
};

/* Keyboard */
class eKeyboard : public eActiveElement
{
public:
    std::vector<String> row1, row2, row3;                      // Буквенная раскладка
    std::vector<String> row1Numbers, row2Numbers, row3Numbers; // Цифровая раскладка

    eKeyboard(std::function<void(char)> onCharInput, int x, int y, int keyW = 18, int keyH = 14)
        : m_onCharInput(onCharInput), m_x(x), m_y(y), m_keyW(keyW), m_keyH(keyH),
          m_lastKeyPressTime(0), m_keyRepeatDelay(200), m_capsLock(false), m_active(true),
          m_useStaticPosition(false), m_staticX(0), m_staticY(0), m_isNumberMode(false)
    {
        // Буквенная раскладка
        row1 = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"};
        row2 = {"A", "S", "D", "F", "G", "H", "J", "K", "L", "Z"};
        row3 = {"12", "X", "C", "V", " ", "B", "N", "M", "BS", "CL"};

        // Цифровая/символьная раскладка
        row1Numbers = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
        row2Numbers = {"-", "/", ":", ";", "(", ")", "$", "&", "@", "\""};
        row3Numbers = {"AB", ".", ",", "?", " ", "!", "'", "%", "BS", "CL"};

        m_zOrder = 500;
    }

    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        if (!m_useStaticPosition)
        {
            this->xForm = x + m_x;
            this->yForm = y + m_y;
        }
        this->wForm = w;
        this->hForm = h;
    }

    // Новый метод для статического позиционирования
    void setStaticPosition(int x, int y)
    {
        m_useStaticPosition = true;
        m_staticX = x;
        m_staticY = y;
        this->xForm = x;
        this->yForm = y;
    }

    void clearStaticPosition()
    {
        m_useStaticPosition = false;
    }

    String getText() const { return m_inputText; }
    void clearText() { m_inputText = ""; }
    void setText(const String &text) { m_inputText = text; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    void setIsActive(bool active)
    {
        m_isActive = active;
        if (!active && g_activeElement == this)
        {
            g_activeElement = nullptr;
        }
    }
    void setKeyRepeatDelay(unsigned long delayMs) { m_keyRepeatDelay = delayMs; }
    bool isCapsLock() const { return m_capsLock; }
    void setCapsLock(bool enabled) { m_capsLock = enabled; }
    void getKeyboardSize(int &width, int &height, int keyW, int keyH) const
    {
        int keySpacing = 2;
        int maxRowSize = max(row1.size(), max(row2.size(), row3.size()));
        width = maxRowSize * (keyW + keySpacing) + keySpacing;
        height = 3 * (keyH + keySpacing) + keySpacing;
    }
    ElementType getElementType() const override { return ELEM_TYPE_KEYBOARD; }

    void toggleNumberMode() 
    { 
        m_isNumberMode = !m_isNumberMode; 
    }
    
    bool isNumberMode() const { return m_isNumberMode; }
    void setNumberMode(bool enabled) { m_isNumberMode = enabled; }

private:
    std::function<void(char)> m_onCharInput;
    String m_inputText;
    bool m_active{true};
    bool m_capsLock;
    int xForm, yForm, wForm, hForm;
    int m_x, m_y;
    int m_keyW, m_keyH;
    unsigned long m_lastKeyPressTime;
    unsigned long m_keyRepeatDelay;

    // Для статического позиционирования
    bool m_useStaticPosition;
    int m_staticX, m_staticY;

    bool m_isNumberMode; // Флаг режима (true - цифры/символы, false - буквы)

    void drawKey(int x, int y, int w, int h, const String &label, bool highlighted);
    bool isKeyPressed(int x, int y, int w, int h);
};

/* Text Input with Keyboard */
class eTextInput : public eActiveElement
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
        m_keyboard->setActive(false);
        m_keyboard->setIsActive(false);
        m_zOrder = 100; // Высокий приоритет для текстовых полей
    }

    ~eTextInput() { delete m_keyboard; }

    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
        if (m_keyboard)
        {
            m_keyboard->setPosition(x, y, w, 0);
        }
    }

    void setText(const String &text)
    {
        m_text = text;
        if (m_keyboard)
            m_keyboard->setText(text);
    }
    String getText() const { return m_text; }
    void clearText()
    {
        m_text = "";
        if (m_keyboard)
            m_keyboard->clearText();
    }
    void setEditing(bool editing) override
    {
        m_isEditing = editing;
        m_isActive = editing;
        if (m_keyboard)
        {
            m_keyboard->setActive(editing);
            m_keyboard->setIsActive(editing);
            if (editing)
            {
                ElementZOrderManager::bringToFront(m_keyboard);
            }
        }
        if (editing)
        {
            g_activeElement = this;
            ElementZOrderManager::bringToFront(this);
        }
        else if (g_activeElement == this)
        {
            g_activeElement = nullptr;
        }
        m_lastToggleTime = millis();
    }
    bool isEditing() const override { return m_isEditing; }
    bool isInEditMode() const override { return m_isEditing; }
    void deactivate() override
    {
        m_isActive = false;
        m_isEditing = false;
        if (m_keyboard)
        {
            m_keyboard->setActive(false);
            m_keyboard->setIsActive(false);
        }
        if (g_activeElement == this)
        {
            g_activeElement = nullptr;
        }
    }
    void setKeyRepeatDelay(unsigned long delayMs)
    {
        if (m_keyboard)
            m_keyboard->setKeyRepeatDelay(delayMs);
    }
    bool isCapsLock() const { return m_keyboard ? m_keyboard->isCapsLock() : false; }
    void setCapsLock(bool enabled)
    {
        if (m_keyboard)
            m_keyboard->setCapsLock(enabled);
    }
    ElementType getElementType() const override { return ELEM_TYPE_TEXT_INPUT; }

    eKeyboard *getKeyboard() const { return m_keyboard; }

private:
    void onCharInput(char ch)
    {
        if (ch == '\b')
        {
            if (m_text.length() > 0)
                m_text.remove(m_text.length() - 1);
        }
        else
        {
            m_text += ch;
        }
        if (m_keyboard)
            m_keyboard->setText(m_text);
        if (m_onTextChanged)
            m_onTextChanged(m_text);
    }

    bool isPointInRect(int x, int y, int rx, int ry, int rw, int rh) const
    {
        return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh);
    }

    String m_label;
    String m_text;
    int m_x, m_y;
    int m_width, m_height;
    int xForm, yForm, wForm, hForm;
    bool m_isEditing;
    eKeyboard *m_keyboard;
    std::function<void(const String &)> m_onTextChanged;
    unsigned long m_lastToggleTime;
    static const unsigned long TOGGLE_COOLDOWN = 250;
};

/* Simple HTML Browser for text-only pages */
class eHtmlBrowser : public eActiveElement
{
public:
    eHtmlBrowser(const String &url, int x, int y, int width, int height)
        : m_url(url), m_x(x), m_y(y), m_width(width), m_height(height),
          m_isLoading(false), m_loadError(false)
    {
        m_zOrder = 20;
        m_textScrollBox = new eTextScrollBox("Loading...", oneLine, width, height, x, y);
        m_textScrollBox->setActive(true);
        m_isActive = true;
        loadUrl(url);
    }

    ~eHtmlBrowser()
    {
        delete m_textScrollBox;
    }

    void show() override
    {
        if (m_textScrollBox)
        {
            m_textScrollBox->show();
        }
    }

    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
        if (m_textScrollBox)
        {
            m_textScrollBox->setPosition(x, y, w, h);
        }
    }

    void setUrl(const String &url)
    {
        m_url = url;
        loadUrl(url);
    }

    String getUrl() const { return m_url; }

    void setText(const String &text)
    {
        if (m_textScrollBox)
        {
            m_textScrollBox->setText(text);
        }
    }

    String getText() const
    {
        return m_textScrollBox ? m_textScrollBox->getText() : "";
    }

    void setActive(bool active) override
    {
        m_isActive = active;
        if (m_textScrollBox)
        {
            m_textScrollBox->setActive(active);
        }
    }

    // // To update the browser with new content
    // void updateBrowserUrl(eHtmlBrowser *browser, const String &newUrl)
    // {
    //     if (browser)
    //     {
    //         browser->setUrl(newUrl);
    //     }
    // }

    // // To manually set text content (for local HTML files)
    // void setBrowserContent(eHtmlBrowser *browser, const String &htmlContent)
    // {
    //     if (browser)
    //     {
    //         browser->setText(htmlContent);
    //     }
    // }

    bool isActiveElement() const override { return true; }
    bool isLoading() const { return m_isLoading; }
    bool hasError() const { return m_loadError; }

private:
    void loadUrl(const String &url)
    {
        m_isLoading = true;
        m_loadError = false;
        
        if (m_textScrollBox)
        {
            m_textScrollBox->setText("Loading: " + url + "...");
        }
        
        String content = fetchPageContent(url);
        
        if (content.isEmpty())
        {
            m_loadError = true;
            if (m_textScrollBox)
            {
                m_textScrollBox->setText("Error: Failed to load page\n\n" + url);
            }
            m_isLoading = false;
            return;
        }
        
        String plainText = extractTextFromHtml(content);
        String transliterated = transliterateRussian(plainText);
        
        if (transliterated.isEmpty())
        {
            if (m_textScrollBox)
            {
                m_textScrollBox->setText("Error: Empty page content\n\n" + url);
            }
        }
        else
        {
            if (m_textScrollBox)
            {
                m_textScrollBox->setText(transliterated);
            }
        }
        
        m_isLoading = false;
    }

    String fetchPageContent(const String &url)
    {
        if (url.isEmpty())
            return "";

        // Проверяем наличие WiFi подключения
        if (WiFi.status() != WL_CONNECTED)
        {
            return "ERROR: WiFi not connected";
        }

        HTTPClient http;
        http.setTimeout(10000); // 10 секунд таймаут
        
        String fullUrl = url;
        if (!url.startsWith("http://") && !url.startsWith("https://"))
        {
            fullUrl = "http://" + url;
        }

        _log("Fetching: " + fullUrl);
        
        http.begin(fullUrl);
        http.addHeader("User-Agent", "ESP32-HTML-Browser/1.0");
        
        int httpCode = http.GET();
        
        if (httpCode <= 0)
        {
            _log("HTTP GET failed: " + String(http.errorToString(httpCode).c_str()));
            http.end();
            return "ERROR: HTTP request failed - " + String(http.errorToString(httpCode).c_str());
        }

        if (httpCode != HTTP_CODE_OK)
        {
            _log("HTTP error: " + String(httpCode));
            String errorMsg = "HTTP Error: " + String(httpCode);
            http.end();
            return errorMsg;
        }

        String payload = http.getString();
        http.end();
        
        _log("Received " + String(payload.length()) + " bytes");
        return payload;
    }

    String extractTextFromHtml(const String &html)
    {
        String result = "";
        bool inTag = false;
        bool inScript = false;
        bool inStyle = false;
        
        for (int i = 0; i < html.length(); i++)
        {
            char c = html[i];
            
            // Пропускаем script и style теги
            if (i < html.length() - 6)
            {
                String tag = html.substring(i, i + 6);
                tag.toLowerCase();
                if (tag == "<scrip" || tag == "<style")
                {
                    inScript = true;
                    inTag = true;
                    continue;
                }
                if (tag == "</scri" || tag == "</styl")
                {
                    inScript = false;
                    while (i < html.length() && html[i] != '>') i++;
                    if (i < html.length()) i++;
                    inTag = false;
                    continue;
                }
            }
            
            if (inScript || inStyle)
                continue;
            
            if (c == '<')
            {
                inTag = true;
                continue;
            }
            
            if (c == '>')
            {
                inTag = false;
                continue;
            }
            
            if (!inTag && c != '\r')
            {
                if (c == '&')
                {
                    String entity = "";
                    while (i < html.length() && html[i] != ';')
                    {
                        entity += html[i];
                        i++;
                    }
                    if (i < html.length() && html[i] == ';')
                    {
                        i++;
                        if (entity == "&lt") result += '<';
                        else if (entity == "&gt") result += '>';
                        else if (entity == "&amp") result += '&';
                        else if (entity == "&quot") result += '"';
                        else if (entity == "&nbsp") result += ' ';
                        else if (entity == "&copy") result += "(c)";
                        else if (entity == "&reg") result += "(R)";
                        else result += entity + ';';
                    }
                }
                else
                {
                    // Обработка пробелов
                    if (c == ' ')
                    {
                        if (result.length() > 0 && result[result.length() - 1] != ' ')
                            result += c;
                    }
                    // Обработка переносов строк
                    else if (c == '\n')
                    {
                        if (result.length() > 0 && result[result.length() - 1] != '\n')
                            result += c;
                    }
                    else if (c == '\t')
                    {
                        result += ' ';
                    }
                    else
                    {
                        result += c;
                    }
                }
            }
        }
        
        // Удаляем множественные переносы строк
        String cleaned = "";
        int newlineCount = 0;
        for (int i = 0; i < result.length(); i++)
        {
            if (result[i] == '\n')
            {
                newlineCount++;
                if (newlineCount <= 2)
                    cleaned += result[i];
            }
            else
            {
                newlineCount = 0;
                cleaned += result[i];
            }
        }
        
        // Обрезаем пробелы в начале и конце
        cleaned.trim();
        
        return cleaned;
    }

    // Транслитерация русского текста в латиницу
    String transliterateRussian(const String &text)
    {
        static const struct { String ru; String en; } translitMap[] = {
            {"А", "A"}, {"а", "a"},
            {"Б", "B"}, {"б", "b"},
            {"В", "V"}, {"в", "v"},
            {"Г", "G"}, {"г", "g"},
            {"Д", "D"}, {"д", "d"},
            {"Е", "E"}, {"е", "e"},
            {"Ё", "Yo"}, {"ё", "yo"},
            {"Ж", "Zh"}, {"ж", "zh"},
            {"З", "Z"}, {"з", "z"},
            {"И", "I"}, {"и", "i"},
            {"Й", "Y"}, {"й", "y"},
            {"К", "K"}, {"к", "k"},
            {"Л", "L"}, {"л", "l"},
            {"М", "M"}, {"м", "m"},
            {"Н", "N"}, {"н", "n"},
            {"О", "O"}, {"о", "o"},
            {"П", "P"}, {"п", "p"},
            {"Р", "R"}, {"р", "r"},
            {"С", "S"}, {"с", "s"},
            {"Т", "T"}, {"т", "t"},
            {"У", "U"}, {"у", "u"},
            {"Ф", "F"}, {"ф", "f"},
            {"Х", "Kh"}, {"х", "kh"},
            {"Ц", "Ts"}, {"ц", "ts"},
            {"Ч", "Ch"}, {"ч", "ch"},
            {"Ш", "Sh"}, {"ш", "sh"},
            {"Щ", "Shch"}, {"щ", "shch"},
            {"Ы", "Y"}, {"ы", "y"},
            {"Э", "E"}, {"э", "e"},
            {"Ю", "Yu"}, {"ю", "yu"},
            {"Я", "Ya"}, {"я", "ya"},
            {"Ь", ""}, {"ь", ""},
            {"Ъ", ""}, {"ъ", ""}
        };

        String result = "";
        int i = 0;
        while (i < text.length())
        {
            bool found = false;
            
            // Пытаемся сопоставить двухбайтовые UTF-8 символы
            if (i + 1 < text.length())
            {
                String twoBytes = text.substring(i, i + 2);
                for (const auto &map : translitMap)
                {
                    if (twoBytes == map.ru)
                    {
                        result += map.en;
                        i += 2;
                        found = true;
                        break;
                    }
                }
            }
            
            if (!found)
            {
                result += text[i];
                i++;
            }
        }
        
        return result;
    }

    void _log(const String &message)
    {
        Serial.println("[eHtmlBrowser] " + message);
    }

    String m_url;
    int m_x, m_y, m_width, m_height;
    int xForm, yForm, wForm, hForm;
    eTextScrollBox *m_textScrollBox;
    bool m_isActive{true};
    bool m_isLoading{false};
    bool m_loadError{false};
};

/* Desktop */
template <typename T>
class eDesktop : public eElement
{
public:
    eDesktop(const std::vector<T> &data) : data_(data)
    {
        m_zOrder = 1;
    }

    void show() override
    {
        uint8_t border{4};
        uint8_t xx{border};
        // uint8_t yy{16};
        uint8_t yy{border};
        Shortcut _shortcutDesktop;
        uint8_t countTask{0}; //1

        for (TaskArguments &t : data_)
        {
            if ((t.activ == false) && (t.bitMap != NULL) && (t.type == DESKTOP))
            {
                _shortcutDesktop.shortcut(t.name, t.bitMap, xx, yy, t.f, _JOY.posX0, _JOY.posY0);
                countTask++;
                xx += (32 + border);

                if (countTask >= 7)
                {
                    xx = border;
                    yy += (32 + border + 16);
                    countTask = 0; // Сбрасываем в 0
                }
            }
        }
    }

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
    eGraphics(void (*func)(int, int, int, int), int x, int y, int w, int h)
        : showFunc(func), m_x(x), m_y(y), m_w(w), m_h(h)
    {
        m_zOrder = 2;
    }

    void show() override { showFunc(xForm, yForm, wForm, hForm); }
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

/* Plotter */
class ePlotter : public eElement
{
public:
    struct DataPoint
    {
        float value;
        unsigned long timestamp;
    };

    ePlotter(int x, int y, int width, int height, unsigned long updateInterval = 50)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_updateInterval(updateInterval)
    {
        m_zOrder = 10;
        m_data.reserve(100);
        m_minValue = 0;
        m_maxValue = 4095;
        m_lastUpdateTime = 0;
    }

    void show() override;
    void setPosition(int x, int y, int w, int h) override
    {
        this->xForm = x + m_x;
        this->yForm = y + m_y;
        this->wForm = w;
        this->hForm = h;
    }

    void addDataPoint(float value)
    {
        // --> update data
        unsigned long currentTime = millis();
        if (currentTime - m_lastUpdateTime < m_updateInterval)
            return;
        m_lastUpdateTime = currentTime;
        // <-- update data

        m_data.push_back({value, millis()});
        if ((int)m_data.size() > m_maxPoints)
        {
            m_data.erase(m_data.begin());
        }

        // Обновляем диапазон
        float minVal = value;
        float maxVal = value;
        for (const auto &p : m_data)
        {
            if (p.value < minVal)
                minVal = p.value;
            if (p.value > maxVal)
                maxVal = p.value;
        }

        float range = maxVal - minVal;
        if (range < 0.001f)
        {
            m_minValue = minVal - 1.0f;
            m_maxValue = maxVal + 1.0f;
        }
        else
        {
            m_minValue = minVal - range * 0.1f;
            m_maxValue = maxVal + range * 0.1f;
        }
    }

    void clearData()
    {
        m_data.clear();
        m_minValue = 0;
        m_maxValue = 4095;
    }

    void setYRange(float min, float max)
    {
        m_minValue = min;
        m_maxValue = max;
    }

    void setMaxPoints(int points) { m_maxPoints = points; }
    void setColor(GRAY::Color color) { m_lineColor = color; }

private:
    int xForm, yForm, wForm, hForm;
    int m_x, m_y, m_width, m_height;
    std::vector<DataPoint> m_data;
    float m_minValue;
    float m_maxValue;
    int m_maxPoints = 100;
    unsigned long m_updateInterval;
    unsigned long m_lastUpdateTime;
    GRAY::Color m_lineColor = GRAY::BLACK;

    void drawFrame();
    void drawGrid();
    void drawData();
    void drawAxisLabels();
};

/* Abstract base class eForm */
class eForm
{
public:
    virtual int showForm() = 0;
    void addElement(eElement *element) { elements.push_back(element); }
    virtual ~eForm()
    {
        for (auto element : elements)
            delete element;
    }

    void sortElementsByZOrder()
    {
        ElementZOrderManager::sortElements(elements);
    }

protected:
    std::vector<eElement *> elements;
};

/* Enums for form modes */
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

/* Глобальные функции для управления свернутыми формами */
extern std::vector<exForm*> minimizedForms;

void minimizeForm(exForm* form);
void restoreForm(exForm* form);
bool isFormMinimized(exForm* form);

/* Class for controlling the glass forms */
class exFormStack
{
public:
    void push(exForm *form) { 
        // Если форма была свернута, восстанавливаем её
        if (isFormMinimized(form))
        {
            restoreForm(form);
        }
        else
        {
            formsStack.push(form); 
        }
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
    
    exForm *top() { 
        if (!formsStack.empty())
            return formsStack.top(); 
        return nullptr;
    }
    
    size_t size() const { return formsStack.size(); }
    bool empty() const { return formsStack.empty(); }

    void refreshForm()
    {
        // Показываем только верхнюю форму из стека
        // Свернутые формы не отображаются
        if (!formsStack.empty())
        {
            exForm *top = formsStack.top();
            // Проверяем, не свернута ли форма
            if (!isFormMinimized(top))
            {
                // Обновляем отображение только если форма не свернута
                top->showForm();
            }
        }
    }

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

/* Глобальные функции для управления активностью */
void resetAllActiveElements(std::vector<eElement *> &elements);
bool isElementGloballyActive(eElement *element);
void deactivateCurrentElement();