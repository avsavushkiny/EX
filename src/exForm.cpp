#include "exForm.h"

std::stack<exForm *> formsStack;

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
// template <typename T>
// void eDesktop<T>::show()
// {
//     uint8_t border{4};
//     uint8_t xx{border};
//     uint8_t yy{16};

//     Shortcut _shortcutDesktop;

//     uint8_t countTask{1};

//     for (TaskArguments &t : data_)
//     {
//         if ((t.activ == false) && (t.bitMap != NULL) && (t.type == DESKTOP))
//         {
//             _shortcutDesktop.shortcut(t.name, t.bitMap, xx, yy, t.f, _JOY.posX0, _JOY.posY0);

//             countTask++;

//             xx += (32 + border);

//             if (countTask > 7)
//             {
//                 xx = 4;
//                 yy += (32 + border + 16);
//                 countTask = 0;
//             }
//         }
//         else
//         {
//             // "There are no tasks to output\nto the desktop."
//         }
//     }
// }

/* Keyboard */
void eKeyboard::show()
{
    if (!m_active) return;

    int keySpacing = 2;
    int keyW = m_keyW;
    int keyH = m_keyH;
    int startX = xForm;
    int startY = yForm;

    // Рисуем фон клавиатуры
    int maxRowSize = max(row1.size(), max(row2.size(), row3.size()));
    int totalWidth = maxRowSize * (keyW + keySpacing) + keySpacing;
    int totalHeight = 3 * (keyH + keySpacing) + keySpacing;
    _GGL.gray.drawFillFrame(startX, startY, totalWidth, totalHeight, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);

    int currentY = startY + keySpacing;

    // Ряд 1 (QWERTYUIOP)
    int row1Width = row1.size() * (keyW + keySpacing) - keySpacing;
    int row1Offset = (totalWidth - row1Width) / 2;
    int currentX = startX + row1Offset;
    for (const auto &key : row1)
    {
        String displayKey = key;
        if (m_capsLock) {
            displayKey.toUpperCase();
        } else {
            displayKey.toLowerCase();
        }
        bool isHighlighted = isKeyPressed(currentX, currentY, keyW, keyH);
        drawKey(currentX, currentY, keyW, keyH, displayKey, isHighlighted);
        currentX += keyW + keySpacing;
    }

    // Ряд 2 (ASDFGHJKLZ)
    currentY += keyH + keySpacing;
    int row2Width = row2.size() * (keyW + keySpacing) - keySpacing;
    int row2Offset = (totalWidth - row2Width) / 2;
    currentX = startX + row2Offset;
    for (const auto &key : row2)
    {
        String displayKey = key;
        if (m_capsLock) {
            displayKey.toUpperCase();
        } else {
            displayKey.toLowerCase();
        }
        bool isHighlighted = isKeyPressed(currentX, currentY, keyW, keyH);
        drawKey(currentX, currentY, keyW, keyH, displayKey, isHighlighted);
        currentX += keyW + keySpacing;
    }

    // Ряд 3 (XCVBNM + Backspace + CapsLock)
    currentY += keyH + keySpacing;
    int row3Width = 0;
    for (const auto &key : row3) {
        int w = keyW;
        if (key == "BS" || key == "CL") {
            w = keyW * 1; // 1.5
        }
        row3Width += w + keySpacing;
    }
    row3Width -= keySpacing;
    int row3Offset = (totalWidth - row3Width) / 2;
    currentX = startX + row3Offset;
    for (const auto &key : row3)
    {
        int w = keyW;
        if (key == "BS" || key == "CL")
        {
            w = keyW * 1; // 1.5
        }
        
        String displayLabel = key;
        if (key == "CL") {
            displayLabel = m_capsLock ? "CL" : "cl";
        } else if (key != "BS" && !key.isEmpty()) {
            if (m_capsLock) {
                displayLabel.toUpperCase();
            } else {
                displayLabel.toLowerCase();
            }
        }
        
        bool isHighlighted = isKeyPressed(currentX, currentY, w, keyH);
        drawKey(currentX, currentY, w, keyH, displayLabel, isHighlighted);
        currentX += w + keySpacing;
    }

    // Обработка нажатий клавиш с использованием таймера
    unsigned long currentTime = millis();
    
    // Проверяем все клавиши первого ряда
    currentY = startY + keySpacing;
    row1Width = row1.size() * (keyW + keySpacing) - keySpacing;
    row1Offset = (totalWidth - row1Width) / 2;
    currentX = startX + row1Offset;
    for (const auto &key : row1)
    {
        if (isKeyPressed(currentX, currentY, keyW, keyH) && _JOY.pressKeyENTER())
        {
            if (currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (m_onCharInput)
                {
                    char ch = key[0];
                    if (m_capsLock) {
                        ch = toupper(ch);
                    } else {
                        ch = tolower(ch);
                    }
                    m_onCharInput(ch);
                    m_inputText += ch;
                }
                m_lastKeyPressTime = currentTime;
            }
        }
        currentX += keyW + keySpacing;
    }

    // Второй ряд
    currentY += keyH + keySpacing;
    row2Width = row2.size() * (keyW + keySpacing) - keySpacing;
    row2Offset = (totalWidth - row2Width) / 2;
    currentX = startX + row2Offset;
    for (const auto &key : row2)
    {
        if (isKeyPressed(currentX, currentY, keyW, keyH) && _JOY.pressKeyENTER())
        {
            if (currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (m_onCharInput)
                {
                    char ch = key[0];
                    if (m_capsLock) {
                        ch = toupper(ch);
                    } else {
                        ch = tolower(ch);
                    }
                    m_onCharInput(ch);
                    m_inputText += ch;
                }
                m_lastKeyPressTime = currentTime;
            }
        }
        currentX += keyW + keySpacing;
    }

    // Третий ряд
    currentY += keyH + keySpacing;
    row3Width = 0;
    for (const auto &key : row3) {
        int w = keyW;
        if (key == "BS" || key == "CL") {
            w = keyW * 1.5;
        }
        row3Width += w + keySpacing;
    }
    row3Width -= keySpacing;
    row3Offset = (totalWidth - row3Width) / 2;
    currentX = startX + row3Offset;
    for (const auto &key : row3)
    {
        int w = keyW;
        if (key == "BS" || key == "CL")
        {
            w = keyW * 1.5;
        }
        
        if (isKeyPressed(currentX, currentY, w, keyH) && _JOY.pressKeyENTER())
        {
            if (currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (key == "BS")
                {
                    // Backspace - удаляем последний символ
                    if (m_inputText.length() > 0)
                    {
                        m_inputText.remove(m_inputText.length() - 1);
                        if (m_onCharInput)
                        {
                            m_onCharInput('\b');
                        }
                    }
                }
                else if (key == "CL")
                {
                    // Caps Lock - переключаем состояние
                    m_capsLock = !m_capsLock;
                    if (m_onCharInput)
                    {
                        // m_onCharInput(m_capsLock ? 'C' : 'c');
                    }
                }
                else if (!key.isEmpty())
                {
                    // Буквенные клавиши
                    if (m_onCharInput)
                    {
                        char ch = key[0];
                        if (m_capsLock) {
                            ch = toupper(ch);
                        } else {
                            ch = tolower(ch);
                        }
                        m_onCharInput(ch);
                        m_inputText += ch;
                    }
                }
                m_lastKeyPressTime = currentTime;
            }
        }
        currentX += w + keySpacing;
    }
}

void eKeyboard::drawKey(int x, int y, int w, int h, const String &label, bool highlighted)
{
    if (highlighted)
    {
        _GGL.gray.drawFillFrame(x, y, w, h, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
        _GGL.gray.writeLine(x + 3, y + 2, label, 10, 1, _GGL.gray.WHITE);
        _GGL.gray.drawFrame(x, y, w, h, _GGL.gray.BLACK);
    }
    else
    {
        _GGL.gray.drawFillFrame(x, y, w, h, _GGL.gray.BLACK, _GGL.gray.WHITE);
        _GGL.gray.writeLine(x + 3, y + 2, label, 10, 1, _GGL.gray.BLACK);
        _GGL.gray.drawFrame(x, y, w, h, _GGL.gray.BLACK);
    }
}

bool eKeyboard::isKeyPressed(int x, int y, int w, int h)
{
    return (_JOY.posX0 >= x && _JOY.posX0 <= x + w &&
            _JOY.posY0 >= y && _JOY.posY0 <= y + h);
}

/* eTextInput */
void eTextInput::show()
{
    unsigned long currentTime = millis();
    
    int inputX = xForm;
    int inputY = yForm;
    int inputW = m_width > 0 ? m_width : 200;
    int inputH = m_height > 0 ? m_height : 20;

    // Рисуем метку
    if (m_label.length() > 0)
    {
        _GGL.gray.writeLine(inputX, inputY - 12, m_label, 10, 1, _GGL.gray.BLACK);
    }

    // Рисуем поле ввода
    if (m_isEditing)
    {
        _GGL.gray.drawFillFrame(inputX, inputY, inputW, inputH, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        _GGL.gray.drawFrame(inputX, inputY, inputW, inputH, _GGL.gray.BLACK);
        _GGL.gray.drawFrame(inputX - 1, inputY - 1, inputW + 2, inputH + 2, _GGL.gray.BLACK);
    }
    else
    {
        _GGL.gray.drawFillFrame(inputX, inputY, inputW, inputH, _GGL.gray.BLACK, _GGL.gray.WHITE);
        _GGL.gray.drawFrame(inputX, inputY, inputW, inputH, _GGL.gray.BLACK);
    }

    // Выводим текст
    String displayText = m_text;
    if (displayText.length() > 0)
    {
        int maxChars = (inputW - 6) / 5;
        if ((int)displayText.length() > maxChars)
        {
            displayText = "..." + displayText.substring(displayText.length() - maxChars + 3);
        }
        _GGL.gray.writeLine(inputX + 3, inputY + 3, displayText, 10, 1, _GGL.gray.BLACK);
    }
    else if (m_isEditing && (currentTime / 500) % 2 == 0)
    {
        // Мигающий курсор
        // _GGL.gray.writeLine(inputX + 3, inputY + 3, "|", 10, 1, _GGL.gray.BLACK);
    }

    // Проверка нажатия ENTER на поле ввода
    bool isCursorOverInput = (m_isEditing == false) && 
        isPointInRect(_JOY.posX0, _JOY.posY0, inputX, inputY, inputW, inputH);
    
    if (isCursorOverInput && _JOY.pressKeyENTER() && (currentTime - m_lastToggleTime >= TOGGLE_COOLDOWN))
    {
        m_isEditing = true;
        if (m_keyboard)
        {
            m_keyboard->setActive(true);
            m_keyboard->setText(m_text);
        }
        m_lastToggleTime = currentTime;
    }

    // Если в режиме редактирования - показываем клавиатуру
    if (m_isEditing && m_keyboard)
    {
        int kbX = xForm;
        int kbY = yForm + m_height + 5;
        int kbW = 0, kbH = 0;
        
        // Получаем размеры клавиатуры
        m_keyboard->getKeyboardSize(kbW, kbH, 18, 14);
        
        m_keyboard->show();
        
        // Проверяем условия выхода из режима редактирования
        // bool isCursorOverInput = isPointInRect(_JOY.posX0, _JOY.posY0, inputX, inputY, inputW, inputH);
        // bool isCursorOverKeyboard = isPointInRect(_JOY.posX0, _JOY.posY0, kbX, kbY, kbW, kbH);
        
        // Выход при клике ENTER вне поля ввода и вне клавиатуры
        // if (!isCursorOverInput && !isCursorOverKeyboard && _JOY.pressKeyENTER() && 
        //     (currentTime - m_lastToggleTime >= TOGGLE_COOLDOWN))
        // {
        //     m_isEditing = false;
        //     if (m_keyboard)
        //     {
        //         m_keyboard->setActive(false);
        //     }
        //     m_lastToggleTime = currentTime;
        // }
        
        // Выход по ESC
        if (_JOY.pressKeyEX() && (currentTime - m_lastToggleTime >= TOGGLE_COOLDOWN))
        {
            m_isEditing = false;
            if (m_keyboard)
            {
                m_keyboard->setActive(false);
            }
            m_lastToggleTime = currentTime;
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
        // if (closeForm.button("CLOSE", 225, 0, _JOY.posX0, _JOY.posY0))
        // {
        //     return 1; // 1 - exit and delete form from stack
        // }

        if (closeForm.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 243, 0, _JOY.posX0, _JOY.posY0))
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
        
        _GRF.print("[" + (String)sizeStack + "]", xSizeStack + 20, 2, 10, 5);
    }

    if (eFormShowMode == MAXIMIZED)
    {
        if (closeForm.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 243, 0, _JOY.posX0, _JOY.posY0))
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
       
       _GRF.print("[" + (String)sizeStack + "]", xSizeStack + 20, 2, 10, 5);
    }

    if (eFormShowMode == NORMAL)
    {
        // if (closeForm.button("CLOSE", 205, outerBoundaryForm - 12 + 6, _JOY.posX0, _JOY.posY0))
        if (closeForm.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 223, outerBoundaryForm - 12 + 6, _JOY.posX0, _JOY.posY0))
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

        _GRF.print("[" + (String)sizeStack + "]", xSizeStack + 20, outerBoundaryForm - 4, 10, 5);
    }

    // выводим все элементы на дисплей
    for (auto element : elements)
    {
        element->show();
    }

    return 0; // 0 - the form works
}