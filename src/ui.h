#pragma once

#include "icon.h"
#include "ggl.h"
#include "input.h"
#include "graphics.h"

extern GGL _GGL;
extern SystemIcon _SICON;
extern Graphics _GRF;

class Cursor
{
private:
public:
    /* Cursor. If the stateCursor status is 1 - visible, if 0 - not visible.
       Determine the coordinates of the Cursor to interact with the selected Stick. */
    bool cursor(bool stateCursor, int xCursor, int yCursor);
};

class Shortcut : Joystick
{
private:
public:
    /* A shortcut on the desktop to launch the void-function.
       Define an icon-image with a resolution of 32x32 pixels + name */
    bool shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
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

class Button : Joystick
{
private:
    int xCursor, yCursor;
public:
    bool button(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor);
};

class Label : Joystick
{
private:
public:
    // Необходимо добавить реализацию нативного Label
};

class InstantMessage
{
public:
    InstantMessage(String text, unsigned int tDelay) : m_text(text), m_delay(tDelay) {}

    void show();
    // void drawDotGrid(int interval);

private:
    // BorderStyle m_borderMessage;
    String m_label, m_text;
    unsigned int m_delay;
    int xForm{0}, yForm{0}, m_sizeW{256}, m_sizeH{160};
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