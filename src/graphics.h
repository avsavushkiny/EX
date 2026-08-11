#pragma once

#include "ggl.h" // Сначала базовые библиотеки
#include "icon.h"

extern GGL _GGL;
extern Icon _ICON;
extern SystemIcon _SICON;

class Graphics
{
private:
public:
    void render(void (*f)())
    {
        _GGL.gray.clearBuffer();
        f();
        _GGL.gray.sendBuffer();
    }

    void print(String text, int x, int y, int8_t lii = 10, int8_t chi = 6)
    {
        int sizeText = text.length();
        int yy{0};

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, text[i], 10, 1, _GGL.gray.BLACK);

            if (text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
    }

    bool waitDisplay()
    {
        static unsigned long lastUpdate = 0;
        static bool isDisplayed = false;
        unsigned long currentTime = millis();

        if (!isDisplayed)
        {
            _GGL.gray.clearBuffer();
            _GGL.gray.bitmap(((W_LCD - _SICON.wait_w) / 2), ((H_LCD - _SICON.wait_h) / 2), _SICON.wait, _SICON.wait_w, _SICON.wait_h, _GGL.gray.NOT_TRANSPARENT);
            _GGL.gray.sendBuffer();
            lastUpdate = currentTime;
            isDisplayed = true;
        }

        if (currentTime - lastUpdate >= 150)
        {
            isDisplayed = false;
            return true;
        }

        return false;
    }

    // далее
};