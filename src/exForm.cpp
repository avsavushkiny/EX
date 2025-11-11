#include "exForm.h"

void eButton::show()
{
    uint8_t sizeText = m_label.length();
    short border{3};
    short charW{5};

    // int x{m_x}, y{m_y + 6 /* offset by Y */};

    if ((_JOY.posX0 >= xForm && _JOY.posX0 <= (xForm + (sizeText * charW) + border + border)) && (_JOY.posY0 >= yForm && _JOY.posY0 <= yForm + 13))
    {
        // ggl.gray.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13, ggl.gray.DARK_GRAY);
        _GGL.gray.drawFillFrame(xForm, yForm, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
        _GGL.gray.writeLine(xForm + border, yForm - 1/* font H */ + border, m_label, 10, 1, _GGL.gray.BLACK);

        if (_JOY.pressKeyENTER() == true)
        {
            _GGL.gray.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK);
            m_stateButton = true;
            m_func(); 
        }
        else m_stateButton = false;
    }
    else
    {
        _GGL.gray.drawFillFrame(xForm, yForm, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.WHITE);
        _GGL.gray.writeLine(xForm + border, yForm - 1 /* font H */ + border, m_label, 10, 1, _GGL.gray.BLACK);
    }
}
/* eText */
void eText::show()
{
    _GRF.print(m_text, xForm, yForm, 10, 5);
    // _GGL.gray.writeLine(xForm, yForm, m_text, 10, 1, _GGL.gray.BLACK);
}
/* eTextbox */
void eTextBox::show()
{
    short border{5}; short border2{8}; // size border
    int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
    short line{1}; // there will always be at least one line of text in the text
    int numberOfCharacters{0}; // количество символов
    short charH{10}, charW{5};
    int ch{0}, ln{0}; int xx{xForm}, yy{yForm}; 
    
    if (m_borderStyle == noBorder){ /* we don't draw anything */ }
    if (m_borderStyle == oneLine)
    {
        _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.BLACK);
    }
    if (m_borderStyle == twoLine)
    {
        _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.BLACK);
        _GGL.gray.drawFrame(xx - 3, yy - 3, m_sizeW + 6, m_sizeH + 6, _GGL.gray.BLACK);
    }
    if (m_borderStyle == shadow)
    {
        _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.BLACK);

        _GGL.gray.drawHLine(xx + 1, yy + m_sizeH, m_sizeW, _GGL.gray.BLACK, 1);
        _GGL.gray.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW, _GGL.gray.BLACK, 1);

        _GGL.gray.drawVLine(xx + m_sizeW, yy + 1, m_sizeH, _GGL.gray.BLACK, 1);
        _GGL.gray.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH, _GGL.gray.BLACK, 1);
    }
    if (m_borderStyle == shadowNoFrame)
    {
        _GGL.gray.drawHLine(xx + 1, yy + m_sizeH, m_sizeW, _GGL.gray.BLACK, 1);
        _GGL.gray.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW, _GGL.gray.BLACK, 1);

        _GGL.gray.drawVLine(xx + m_sizeW, yy + 1, m_sizeH, _GGL.gray.BLACK, 1);
        _GGL.gray.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH, _GGL.gray.BLACK, 1);
    }
    
    for (char c : m_text)
    {
        numberOfCharacters++;
    }

    int numberOfCharactersLineFrame = (m_sizeW - border - border) / charW; // количество символов в строчке Фрейма
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
    int numberOfLinesFrame = (m_sizeH - border - border) / charH; // количество строчек в Фрейме
    
    for (char c : m_text)
    {
        // ggl.gray.writeLine(xx + border, yy + charH + border, c, 10, 1, ggl.gray.BLACK);
        _GGL.gray.writeChar(xx + border, yy + border, c, 10, 1, _GGL.gray.BLACK);
        
        xx += charW;
        ch++;

        if (c == '\n')
        {
            yy += charH; ch = 0; xx = xForm; ln++;
        }

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH; ch = 0; xx = xForm; ln++;
        }
    }
}
/* eLabel */
void eLabel::show()
{
    uint8_t sizeText = m_text.length();
    uint8_t chi{5}, lii{8};
    int x{xForm}, y{yForm};

    for (int i = 0; i < sizeText; i++)
    { 
        x += chi;
        _GGL.gray.writeChar(x, y, m_text[i], 10, 1, _GGL.gray.BLACK);

        if (m_text[i] == '\n')
        {
            x = xForm; y += lii;
        }
    }
}
/* eLabel with a link */
void eLinkLabel::show()
{
    uint8_t sizeText = m_text.length();
    uint8_t yy{}, chi{5}, lii{8}; int x{xForm + 1}, y{yForm};

    if ((_JOY.posX0 >= x && _JOY.posX0 <= (x + (sizeText * chi))) && (_JOY.posY0 >= y - 2 && _JOY.posY0 <= y + lii + 2))
    {
        _GGL.gray.drawBox(x - 1, y, (sizeText * chi) + 2, lii + 1, _GGL.gray.BLACK);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.WHITE);

            if (m_text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }

        if (_JOY.pressKeyENTER() == true)
        {
            for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
            {
                _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.DARK_GRAY);
    
                if (m_text[i] == '\n')
                {
                    yy += lii; // 10
                    xx = -chi; // 6
                }
            }
            
            m_onClick();
        }
    }
    else
    {
        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.BLACK);

            if (m_text[i] == '\n')
            {
                yy += lii; // 10
                xx = -chi; // 6
            }
        }
    }
}
/* Horizontal line */
void eLine::show()
{
    _GGL.gray.drawHLine(xForm, yForm, wForm, _GGL.gray.BLACK, 1);
}
/* eCheckbox */
void eCheckbox::show()
{
    // рисуем фрейм и выводим текст
    if (m_checked == true)
    {
        _GGL.gray.drawFillFrame(xForm, yForm, 10, 10, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
    }

    if (m_checked == false)
    {
        _GGL.gray.drawFillFrame(xForm, yForm, 10, 10, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
    }

    _GGL.gray.writeLine(xForm + 15, yForm, m_text, 10, 1, _GGL.gray.BLACK);
    // если курсор над фреймом, то ждем нажатия на кнопку Ввода
    if ((_JOY.posX0 >= xForm) && (_JOY.posX0 <= xForm + 10) && ((_JOY.posY0 >= yForm) && (_JOY.posY0 <= yForm + 10)))
    {
        _GGL.gray.drawBox(xForm, yForm, 10, 10, _GGL.gray.BLACK);

        if (_JOY.pressKeyENTER() == true)
        {
            // если кнопка нажата
            // - если чекед 1 то меняем на 0
            // - если чекед 0 то меняем на 1
            switch (m_checked)
            {
            case true:
                m_checked = false;
                delay(250);
                break;
            case false:
                m_checked = true;
                delay(250);
                break;
            }
        }
    }
}
/* eFunction */
void eFunction::show()
{
    m_func();
}
/* ePicture */
void ePicture::show()
{
    _GGL.gray.bitmap(xForm, yForm, m_bitmap, m_w, m_h, _GGL.gray.NOT_TRANSPARENT);
}
/* eBackground */
void eBackground::show()
{
    // Проходим по всему окну с шагом, равным размеру изображения
    for (int y = yForm; y < 148; y += m_h)
    {
        for (int x = xForm; x < 256; x += m_w)
        {
            // Выводим изображение на текущей позиции (x, y)
            _GGL.gray.bitmap(x, y, m_bitmap, m_w, m_h, _GGL.gray.TRANSPARENT);
        }
    }
}
/* desktop */
template <typename T>
void eDesktop<T>::show()
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
        else
        {
            // "There are no tasks to output\nto the desktop."
        }
    }
}
/* exForm show */
int exForm::showForm()
{
    Button closeForm;

    int sizeStack = formsStack.size();

    switch (eFormShowMode)
    {
    case FULLSCREEN:
        // Для полноэкранного режима устанавливаем координаты относительно экрана
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x, element->m_y + 12, element->m_w, element->m_h);
        }
        break;
    case MAXIMIZED:
        // Для максимизированного режима устанавливаем координаты относительно окна
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x, element->m_y + 12, element->m_w, element->m_h);
        }
        break;
    case NORMAL:
        // Для нормального режима оставляем координаты без изменений
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x + 20, element->m_y + 26, element->m_w - 40, element->m_h);
        }
        break;
    }

    if (eFormShowMode == FULLSCREEN)
    {
        if (closeForm.button("CLOSE", 225, 0, _JOY.posX0, _JOY.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        switch (eFormBackground)
        {
            case TRANSPARENT:
            _GGL.gray.drawFrame(0, 12, 256, 148, _GGL.gray.BLACK);
            break;
            case WHITE:
            _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.WHITE);
            break;
            case LIGHT_GRAY:
            _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
            break;
            case DARK_GRAY:
            _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            break;
            case BLACK:
            _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.BLACK);
            break;
            default:
            _GGL.gray.drawFrame(0, 12, 256, 148, _GGL.gray.BLACK);
            break;
        }

        // ggl.gray.drawFrame(0, 12, 256, 148, ggl.gray.BLACK);
        _GRF.print(title, 5, 2, 10, 5); // size Font, text, x, y, lii, chi
        
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 205;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 200;
        }
        
        _GRF.print("[" + (String)sizeStack + "]", xSizeStack, 2, 10, 5);
    }

    if (eFormShowMode == MAXIMIZED)
    {
        if (closeForm.button("CLOSE", 225, 0, _JOY.posX0, _JOY.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }
        
        switch (eFormBackground)
        {
            case TRANSPARENT:
            _GGL.gray.drawFrame(0, 12, 256, 137, _GGL.gray.BLACK);
            break;
            case WHITE:
            _GGL.gray.drawFillFrame(0, 12, 256, 137, _GGL.gray.BLACK, _GGL.gray.WHITE);
            break;
            case LIGHT_GRAY:
            _GGL.gray.drawFillFrame(0, 12, 256, 137, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
            break;
            case DARK_GRAY:
            _GGL.gray.drawFillFrame(0, 12, 256, 137, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            break;
            case BLACK:
            _GGL.gray.drawFillFrame(0, 12, 256, 137, _GGL.gray.BLACK, _GGL.gray.BLACK);
            break;
            default:
            _GGL.gray.drawFrame(0, 12, 256, 137, _GGL.gray.BLACK);
            break;
        }
        
        // ggl.gray.drawFrame(0, 12, 256, 137, ggl.gray.BLACK);
        _GRF.print(title, 5, 2, 10, 5); // size Font, text, x, y, lii, chi
       
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 205;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 200;
        }
       
       _GRF.print("[" + (String)sizeStack + "]", xSizeStack, 2, 10, 5);
    }

    if (eFormShowMode == NORMAL)
    {

        if (closeForm.button("CLOSE", 205, outerBoundaryForm - 12 + 6, _JOY.posX0, _JOY.posY0))
        {
            return 1; // 1 - exit and delete form from stack
        }

        switch (eFormBackground)
        {
            case TRANSPARENT:
            _GGL.gray.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, _GGL.gray.BLACK);
            break;
            case WHITE:
            _GGL.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, _GGL.gray.BLACK, _GGL.gray.WHITE);
            break;
            case LIGHT_GRAY:
            _GGL.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
            break;
            case DARK_GRAY:
            _GGL.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            break;
            case BLACK:
            _GGL.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, _GGL.gray.BLACK, _GGL.gray.BLACK);
            break;
            default:
            _GGL.gray.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, _GGL.gray.BLACK);
            break;
        }
        
        // ggl.gray.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120, ggl.gray.BLACK);
        _GRF.print(title, outerBoundaryForm + 5, outerBoundaryForm - 4, 10, 5);
        
        uint8_t xSizeStack{};
        
        if (sizeStack <= 9)
        {
            xSizeStack = 185;
        }
        if ((sizeStack >= 10) && (sizeStack <= 99))
        {
            xSizeStack = 180;
        }

        _GRF.print("[" + (String)sizeStack + "]", xSizeStack, outerBoundaryForm - 4, 10, 5);
    }

    // выводим все элементы на дисплей
    for (auto element : elements)
    {
        element->show();
    }

    return 0; // 0 - the form works
}