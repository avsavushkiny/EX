#pragma once

#include "icon.h"
#include "ggl.h"

extern GGL _GGL;
extern SystemIcon _SICON;

class Cursor
{
private:
public:
    /* Cursor. If the stateCursor status is 1 - visible, if 0 - not visible.
       Determine the coordinates of the Cursor to interact with the selected Stick. */
    bool cursor(bool stateCursor, int xCursor, int yCursor);
};

// class Shortcut : Joystick
// {
// private:
// public:
//     /* A shortcut on the desktop to launch the void-function.
//        Define an icon-image with a resolution of 32x32 pixels + name */
//     bool shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor);
// };

// class Label : Joystick
// {
// private:
// public:
// };

// class TextBox
// {
// private:
// public:
//     /* Properties Text-box */
//     enum objectLocation {left, middle, right};
//     enum objectBoundary {noBorder, oneLine, twoLine, shadow, shadowNoFrame};

//     void textBox(String str, int sizeH, int sizeW, short charH, short charW, int x, int y);
// };