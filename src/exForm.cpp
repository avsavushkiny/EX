#include "exForm.h"

std::stack<exForm *> formsStack;

/* Helper function to check if point is over any higher element */
static bool isPointOverHigherElement(const std::vector<eElement *> &allElements,
                                     const eElement *currentElement,
                                     int x, int y)
{
    for (auto *other : allElements)
    {
        if (other == currentElement)
            continue;
        if (!other->m_visible || !other->m_enabled)
            continue;
        if (other->getZIndex() > currentElement->getZIndex() &&
            other->isPointOverElement(x, y))
        {
            return true;
        }
    }
    return false;
}

/* eButton */
void eButton::show()
{
    // Для обратной совместимости
    std::vector<eElement *> emptyList;
    show(emptyList);
}

void eButton::show(const std::vector<eElement *> &allElements)
{
    if (!m_visible || !m_enabled)
        return;

    uint8_t sizeText = m_label.length();
    short border{3};
    short charW{5};

    // Проверяем, находится ли курсор над кнопкой
    bool isCursorOver = (_JOY.posX0 >= m_x && _JOY.posX0 <= (m_x + (sizeText * charW) + border + border)) &&
                        (_JOY.posY0 >= m_y && _JOY.posY0 <= m_y + 13);

    // Проверяем, не перекрыта ли кнопка более верхними элементами
    bool isOverlapped = false;
    if (isCursorOver)
    {
        isOverlapped = isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
    }

    if (isCursorOver && !isOverlapped)
    {
        // Кнопка подсвечена и не перекрыта
        _GGL.gray.drawFillFrame(m_x, m_y, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
        _GGL.gray.writeLine(m_x + border, m_y - 1 + border, m_label, 10, 1, _GGL.gray.BLACK);

        if (_JOY.pressKeyENTER() == true)
        {
            _GGL.gray.drawBox(m_x, m_y, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK);
            m_stateButton = true;
            m_func();
        }
        else
        {
            m_stateButton = false;
        }
    }
    else
    {
        // Обычное состояние кнопки
        _GGL.gray.drawFillFrame(m_x, m_y, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.WHITE);
        _GGL.gray.writeLine(m_x + border, m_y - 1 + border, m_label, 10, 1, _GGL.gray.BLACK);
    }
}

/* eText */
void eText::show()
{
    if (!m_visible)
        return;
    _GRF.print(m_text, m_x, m_y, 10, 5);
}

/* eTextBox */
void eTextBox::show()
{
    if (!m_visible)
        return;

    short border{5};
    int count{0};
    int countChars{0};
    int maxChar{0};
    short line{1};
    int numberOfCharacters{0};
    short charH{10}, charW{5};
    int ch{0}, ln{0};
    int xx{m_x}, yy{m_y};

    if (m_borderStyle == noBorder)
    { /* we don't draw anything */
    }
    else if (m_borderStyle == oneLine)
    {
        _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.BLACK);
    }
    else if (m_borderStyle == twoLine)
    {
        _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.BLACK);
        _GGL.gray.drawFrame(xx - 3, yy - 3, m_sizeW + 6, m_sizeH + 6, _GGL.gray.BLACK);
    }
    else if (m_borderStyle == shadow)
    {
        _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.BLACK);
        _GGL.gray.drawHLine(xx + 1, yy + m_sizeH, m_sizeW, _GGL.gray.BLACK, 1);
        _GGL.gray.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW, _GGL.gray.BLACK, 1);
        _GGL.gray.drawVLine(xx + m_sizeW, yy + 1, m_sizeH, _GGL.gray.BLACK, 1);
        _GGL.gray.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH, _GGL.gray.BLACK, 1);
    }
    else if (m_borderStyle == shadowNoFrame)
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

    int numberOfCharactersLineFrame = (m_sizeW - border - border) / charW;
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame;
    int numberOfLinesFrame = (m_sizeH - border - border) / charH;

    for (char c : m_text)
    {
        _GGL.gray.writeChar(xx + border, yy + border, c, 10, 1, _GGL.gray.BLACK);

        xx += charW;
        ch++;

        if (c == '\n')
        {
            yy += charH;
            ch = 0;
            xx = m_x;
            ln++;
        }

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH;
            ch = 0;
            xx = m_x;
            ln++;
        }
    }
}

/* eLabel */
void eLabel::show()
{
    if (!m_visible)
        return;

    uint8_t sizeText = m_text.length();
    uint8_t chi{5}, lii{8};
    int x{m_x}, y{m_y};

    for (int i = 0; i < sizeText; i++)
    {
        x += chi;
        _GGL.gray.writeChar(x, y, m_text[i], 10, 1, _GGL.gray.BLACK);

        if (m_text[i] == '\n')
        {
            x = m_x;
            y += lii;
        }
    }
}

/* eLinkLabel */
void eLinkLabel::show()
{
    std::vector<eElement *> emptyList;
    show(emptyList);
}

void eLinkLabel::show(const std::vector<eElement *> &allElements)
{
    if (!m_visible || !m_enabled)
        return;

    uint8_t sizeText = m_text.length();
    uint8_t yy{}, chi{5}, lii{8};
    int x{m_x + 1}, y{m_y};

    bool isCursorOver = (_JOY.posX0 >= x && _JOY.posX0 <= (x + (sizeText * chi))) &&
                        (_JOY.posY0 >= y - 2 && _JOY.posY0 <= y + lii + 2);

    bool isOverlapped = false;
    if (isCursorOver)
    {
        isOverlapped = isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
    }

    if (isCursorOver && !isOverlapped)
    {
        _GGL.gray.drawBox(x - 1, y, (sizeText * chi) + 2, lii + 1, _GGL.gray.BLACK);

        for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.WHITE);

            if (m_text[i] == '\n')
            {
                yy += lii;
                xx = -chi;
            }
        }

        if (_JOY.pressKeyENTER() == true)
        {
            for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
            {
                _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.DARK_GRAY);

                if (m_text[i] == '\n')
                {
                    yy += lii;
                    xx = -chi;
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
                yy += lii;
                xx = -chi;
            }
        }
    }
}

/* eLine */
void eLine::show()
{
    if (!m_visible)
        return;
    _GGL.gray.drawHLine(m_x, m_y, m_w, _GGL.gray.BLACK, 1);
}

/* eCheckbox */
void eCheckbox::show()
{
    std::vector<eElement *> emptyList;
    show(emptyList);
}

void eCheckbox::show(const std::vector<eElement *> &allElements)
{
    if (!m_visible || !m_enabled)
        return;

    // Рисуем фрейм и выводим текст
    if (m_checked == true)
    {
        _GGL.gray.drawFillFrame(m_x, m_y, 10, 10, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
    }
    else
    {
        _GGL.gray.drawFillFrame(m_x, m_y, 10, 10, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
    }

    _GGL.gray.writeLine(m_x + 15, m_y, m_text, 10, 1, _GGL.gray.BLACK);

    // Проверяем, находится ли курсор над фреймом
    bool isCursorOver = (_JOY.posX0 >= m_x) && (_JOY.posX0 <= m_x + 10) &&
                        (_JOY.posY0 >= m_y) && (_JOY.posY0 <= m_y + 10);

    bool isOverlapped = false;
    if (isCursorOver)
    {
        isOverlapped = isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
    }

    if (isCursorOver && !isOverlapped)
    {
        _GGL.gray.drawBox(m_x, m_y, 10, 10, _GGL.gray.BLACK);

        if (_JOY.pressKeyENTER() == true)
        {
            m_checked = !m_checked;
            delay(250);
        }
    }
}

/* eFunction */
void eFunction::show()
{
    if (!m_visible || !m_enabled)
        return;
    m_func();
}

/* ePicture */
void ePicture::show()
{
    if (!m_visible)
        return;
    _GGL.gray.bitmap(m_x, m_y, m_bitmap, m_w, m_h, _GGL.gray.NOT_TRANSPARENT);
}

/* eBackground */
void eBackground::show()
{
    if (!m_visible)
        return;

    for (int y = m_y; y < 148; y += m_h)
    {
        for (int x = m_x; x < 256; x += m_w)
        {
            _GGL.gray.bitmap(x, y, m_bitmap, m_w, m_h, _GGL.gray.TRANSPARENT);
        }
    }
}

/* eKeyboard */
void eKeyboard::show()
{
    std::vector<eElement *> emptyList;
    show(emptyList);
}

void eKeyboard::show(const std::vector<eElement *> &allElements)
{
    if (!m_active || !m_visible)
        return;

    int keySpacing = 2;
    int keyW = m_keyW;
    int keyH = m_keyH;
    int startX = m_x;
    int startY = m_y;

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
        if (m_capsLock)
        {
            displayKey.toUpperCase();
        }
        else
        {
            displayKey.toLowerCase();
        }
        bool isHighlighted = isKeyPressed(currentX, currentY, keyW, keyH);
        // Проверяем перекрытие только если клавиша подсвечена
        if (isHighlighted)
        {
            isHighlighted = !isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
        }
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
        if (m_capsLock)
        {
            displayKey.toUpperCase();
        }
        else
        {
            displayKey.toLowerCase();
        }
        bool isHighlighted = isKeyPressed(currentX, currentY, keyW, keyH);
        if (isHighlighted)
        {
            isHighlighted = !isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
        }
        drawKey(currentX, currentY, keyW, keyH, displayKey, isHighlighted);
        currentX += keyW + keySpacing;
    }

    // Ряд 3 (XCVBNM + Backspace + CapsLock)
    currentY += keyH + keySpacing;
    int row3Width = 0;
    for (const auto &key : row3)
    {
        int w = keyW;
        if (key == "BS" || key == "CL")
        {
            w = keyW * 1.5;
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
            w = keyW * 1.5;
        }

        String displayLabel = key;
        if (key == "CL")
        {
            displayLabel = m_capsLock ? "CL" : "cl";
        }
        else if (key != "BS" && !key.isEmpty())
        {
            if (m_capsLock)
            {
                displayLabel.toUpperCase();
            }
            else
            {
                displayLabel.toLowerCase();
            }
        }

        bool isHighlighted = isKeyPressed(currentX, currentY, w, keyH);
        if (isHighlighted)
        {
            isHighlighted = !isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
        }
        drawKey(currentX, currentY, w, keyH, displayLabel, isHighlighted);
        currentX += w + keySpacing;
    }

    // Обработка нажатий клавиш
    unsigned long currentTime = millis();

    // Проверяем все клавиши первого ряда
    currentY = startY + keySpacing;
    row1Width = row1.size() * (keyW + keySpacing) - keySpacing;
    row1Offset = (totalWidth - row1Width) / 2;
    currentX = startX + row1Offset;
    for (const auto &key : row1)
    {
        bool isPressed = isKeyPressed(currentX, currentY, keyW, keyH);
        if (isPressed)
        {
            bool isOverlapped = isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
            if (!isOverlapped && _JOY.pressKeyENTER() && currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (m_onCharInput)
                {
                    char ch = key[0];
                    if (m_capsLock)
                    {
                        ch = toupper(ch);
                    }
                    else
                    {
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
        bool isPressed = isKeyPressed(currentX, currentY, keyW, keyH);
        if (isPressed)
        {
            bool isOverlapped = isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
            if (!isOverlapped && _JOY.pressKeyENTER() && currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (m_onCharInput)
                {
                    char ch = key[0];
                    if (m_capsLock)
                    {
                        ch = toupper(ch);
                    }
                    else
                    {
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
    for (const auto &key : row3)
    {
        int w = keyW;
        if (key == "BS" || key == "CL")
        {
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

        bool isPressed = isKeyPressed(currentX, currentY, w, keyH);
        if (isPressed)
        {
            bool isOverlapped = isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
            if (!isOverlapped && _JOY.pressKeyENTER() && currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (key == "BS")
                {
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
                    m_capsLock = !m_capsLock;
                }
                else if (!key.isEmpty())
                {
                    if (m_onCharInput)
                    {
                        char ch = key[0];
                        if (m_capsLock)
                        {
                            ch = toupper(ch);
                        }
                        else
                        {
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
    std::vector<eElement *> emptyList;
    show(emptyList);
}

void eTextInput::show(const std::vector<eElement *> &allElements)
{
    if (!m_visible)
        return;

    unsigned long currentTime = millis();

    int inputX = m_x;
    int inputY = m_y;
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

    // Проверка нажатия ENTER на поле ввода
    bool isCursorOverInput = (m_isEditing == false) &&
                             isPointInRect(_JOY.posX0, _JOY.posY0, inputX, inputY, inputW, inputH);

    // Проверяем, не перекрыто ли поле ввода более верхними элементами
    if (isCursorOverInput)
    {
        isCursorOverInput = !isPointOverHigherElement(allElements, this, _JOY.posX0, _JOY.posY0);
    }

    if (isCursorOverInput && _JOY.pressKeyENTER() && (currentTime - m_lastToggleTime >= TOGGLE_COOLDOWN))
    {
        m_isEditing = true;
        if (m_keyboard)
        {
            m_keyboard->setActive(true);
            m_keyboard->setText(m_text);
            m_keyboard->setZIndex(100); // Клавиатура всегда сверху
        }
        m_lastToggleTime = currentTime;
    }

    // Если в режиме редактирования - показываем клавиатуру
    if (m_isEditing && m_keyboard)
    {
        // Убеждаемся, что клавиатура имеет высокий Z-индекс
        m_keyboard->setZIndex(100);

        // Получаем список всех элементов для проверки перекрытия
        std::vector<eElement *> allElementsWithKeyboard = allElements;
        allElementsWithKeyboard.push_back(m_keyboard);

        m_keyboard->show(allElementsWithKeyboard);

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
        for (const auto &element : elements)
        {
            element->setPosition(0, 12, 256, 148);
        }
        break;
    case MAXIMIZED:
        for (const auto &element : elements)
        {
            element->setPosition(0, 12, 256, 137);
        }
        break;
    case NORMAL:
        for (const auto &element : elements)
        {
            element->setPosition(20, 26, 216, 120);
        }
        break;
    }

    // Сортируем элементы по Z-индексу
    sortElementsByZIndex();

    if (eFormShowMode == FULLSCREEN)
    {
        if (closeForm.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 243, 0, _JOY.posX0, _JOY.posY0))
        {
            return 1;
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

        _GRF.print(title, 5, 2, 10, 5);

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
            return 1;
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

        _GRF.print(title, 5, 2, 10, 5);

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
        if (closeForm.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 223, outerBoundaryForm - 12 + 6, _JOY.posX0, _JOY.posY0))
        {
            return 1;
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

    // Получаем список всех элементов для проверки перекрытия
    const std::vector<eElement *> &allElements = getElements();

    // Выводим все элементы на дисплей с учетом перекрытия
    for (auto element : elements)
    {
        if (element)
        {
            // Передаем список всех элементов для проверки перекрытия
            element->show(allElements);
        }
    }

    return 0;
}