#include "exForm.h"

std::stack<exForm *> formsStack;
std::vector<exForm*> minimizedForms;
eElement *g_activeElement = nullptr;

/* Глобальные функции для управления активностью */
void resetAllActiveElements(std::vector<eElement *> &elements)
{
    for (auto element : elements)
    {
        if (element->isActiveElement())
        {
            element->setActive(true);
        }
    }
    g_activeElement = nullptr;
}

bool isElementGloballyActive(eElement *element)
{
    return (g_activeElement == nullptr || g_activeElement == element);
}

void deactivateCurrentElement()
{
    if (g_activeElement != nullptr)
    {
        g_activeElement->deactivate();
        g_activeElement = nullptr;
    }
}

/* Функции для управления свернутыми формами */
void minimizeForm(exForm* form)
{
    if (form == nullptr) return;
    
    // Проверяем, не свернута ли уже форма
    auto it = std::find(minimizedForms.begin(), minimizedForms.end(), form);
    if (it != minimizedForms.end()) return;
    
    // Если форма в стеке, удаляем её оттуда
    std::stack<exForm*> tempStack;
    while (!formsStack.empty())
    {
        exForm* top = formsStack.top();
        formsStack.pop();
        if (top != form)
        {
            tempStack.push(top);
        }
    }
    while (!tempStack.empty())
    {
        formsStack.push(tempStack.top());
        tempStack.pop();
    }
    
    // Добавляем в список свернутых
    minimizedForms.push_back(form);
}

void restoreForm(exForm* form)
{
    if (form == nullptr) return;
    
    // Удаляем из списка свернутых
    auto it = std::find(minimizedForms.begin(), minimizedForms.end(), form);
    if (it != minimizedForms.end())
    {
        minimizedForms.erase(it);
        // Возвращаем форму в стек
        formsStack.push(form);
    }
}

bool isFormMinimized(exForm* form)
{
    if (form == nullptr) return false;
    auto it = std::find(minimizedForms.begin(), minimizedForms.end(), form);
    return (it != minimizedForms.end());
}

/* eButton */
void eButton::show()
{
    if (!m_isActive)
    {
        _GGL.gray.drawFillFrame(xForm, yForm, (m_label.length() * 5) + 3 + 3, 13, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        _GGL.gray.writeLine(xForm + 3, yForm - 1 + 3, m_label, 10, 1, _GGL.gray.DARK_GRAY);
        return;
    }

    uint8_t sizeText = m_label.length();
    short border{3};
    short charW{5};

    if ((_JOY.posX0 >= xForm && _JOY.posX0 <= (xForm + (sizeText * charW) + border + border)) &&
        (_JOY.posY0 >= yForm && _JOY.posY0 <= yForm + 13))
    {
        _GGL.gray.drawFillFrame(xForm, yForm, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
        _GGL.gray.writeLine(xForm + border, yForm - 1 + border, m_label, 10, 1, _GGL.gray.BLACK);

        if (g_activeElement == nullptr || g_activeElement == this)
        {
            if (_JOY.pressKeyENTER() == true)
            {
                _GGL.gray.drawBox(xForm, yForm, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK);
                m_stateButton = true;
                m_func();
                if (g_activeElement == this)
                {
                    g_activeElement = nullptr;
                }
            }
            else
                m_stateButton = false;
        }
    }
    else
    {
        _GGL.gray.drawFillFrame(xForm, yForm, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.WHITE);
        _GGL.gray.writeLine(xForm + border, yForm - 1 + border, m_label, 10, 1, _GGL.gray.BLACK);
    }
}

/* eText */
void eText::show()
{
    _GRF.print(m_text, xForm, yForm, 10, 5);
}

/* eTextBox */
void eTextBox::show()
{
    short border{5};
    short charH{10}, charW{5};
    int ch{0}, ln{0};
    int xx{xForm}, yy{yForm};

    bool isActive = m_isActive;

    if (!isActive)
    {
        if (m_borderStyle == oneLine || m_borderStyle == twoLine || m_borderStyle == shadow)
        {
            _GGL.gray.drawFrame(xx, yy, m_sizeW, m_sizeH, _GGL.gray.DARK_GRAY);
        }
        for (char c : m_text)
        {
            _GGL.gray.writeChar(xx + border, yy + border, c, 10, 1, _GGL.gray.DARK_GRAY);
            xx += charW;
            ch++;
            if (c == '\n' || ch >= (m_sizeW - border - border) / charW)
            {
                yy += charH;
                ch = 0;
                xx = xForm;
                ln++;
            }
        }
        return;
    }

    if (m_borderStyle == noBorder)
    { /* ничего не рисуем */
    }
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

    int numberOfCharacters = m_text.length();
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
            xx = xForm;
            ln++;
        }
        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH;
            ch = 0;
            xx = xForm;
            ln++;
        }
    }
}

/* eTextScrollBox */
void eTextScrollBox::show()
{
    short border{0};  // Уменьшенный отступ
    short charH{10}, charW{5};
    int scrollBarX, scrollBarY, scrollBarH;
    int contentW = m_sizeW - SCROLL_BAR_WIDTH - border - border;
    int contentH = m_sizeH - border - border;
    int charsPerLine = contentW / charW;
    int linesVisible = contentH / charH;
    
    // Проверяем, активен ли элемент
    bool isActive = m_isActive;

    // // Рисуем рамку вокруг всего компонента (тонкая линия)
    // if (!isActive)
    // {
    //     _GGL.gray.drawFrame(xForm, yForm, m_sizeW, m_sizeH, _GGL.gray.DARK_GRAY);
    //     // Рисуем текст в неактивном состоянии
    //     drawTextWithScroll();
    //     return;
    // }

    // Рисуем тонкую рамку в активном состоянии
    _GGL.gray.drawFrame(xForm, yForm, m_sizeW, m_sizeH, _GGL.gray.BLACK);

    // Рисуем текст с прокруткой
    drawTextWithScroll();

    // Рисуем скролл-бар (вплотную к правому краю)
    scrollBarX = xForm + m_sizeW - SCROLL_BAR_WIDTH - 2;
    scrollBarY = yForm + border;
    scrollBarH = m_sizeH - border * 2;
    drawScrollBar(scrollBarX, scrollBarY, scrollBarH);

    // Обработка нажатий для прокрутки
    unsigned long currentTime = millis();
    bool canScroll = (currentTime - m_lastScrollTime >= SCROLL_DELAY);

    // Проверяем наведение на верхнюю часть скролл-бара (стрелка вверх)
    int upArrowY1 = scrollBarY;
    int upArrowY2 = scrollBarY + SCROLL_BAR_WIDTH + 2;
    
    // Проверяем наведение на нижнюю часть скролл-бара (стрелка вниз)
    int downArrowY1 = scrollBarY + scrollBarH - SCROLL_BAR_WIDTH - 2;
    int downArrowY2 = scrollBarY + scrollBarH;

    if (g_activeElement == nullptr || g_activeElement == this)
    {
        bool isOverUp = isPointInRect(_JOY.posX0, _JOY.posY0, scrollBarX, upArrowY1, SCROLL_BAR_WIDTH, SCROLL_BAR_WIDTH + 2);
        bool isOverDown = isPointInRect(_JOY.posX0, _JOY.posY0, scrollBarX, downArrowY1, SCROLL_BAR_WIDTH, SCROLL_BAR_WIDTH + 2);

        // Подсветка стрелок при наведении
        if (isOverUp && m_scrollOffset > 0)
        {
            _GGL.gray.drawFillFrame(scrollBarX + 1, upArrowY1 + 1, SCROLL_BAR_WIDTH - 2, SCROLL_BAR_WIDTH, 
                                    _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            // Рисуем стрелку вверх (треугольник)
            int cx = scrollBarX + SCROLL_BAR_WIDTH / 2;
            int cy = upArrowY1 + SCROLL_BAR_WIDTH / 2;
            for (int i = 0; i < SCROLL_BAR_WIDTH / 2 - 1; i++)
            {
                int x1 = cx - i;
                int x2 = cx + i;
                if (i > 0)
                {
                    _GGL.gray.drawPixel(x1, cy - i, _GGL.gray.WHITE);
                    _GGL.gray.drawPixel(x2, cy - i, _GGL.gray.WHITE);
                }
                _GGL.gray.drawPixel(cx, cy - SCROLL_BAR_WIDTH / 2 + 1, _GGL.gray.WHITE);
            }
        }
        else if (isOverUp)
        {
            _GGL.gray.drawFillFrame(scrollBarX + 1, upArrowY1 + 1, SCROLL_BAR_WIDTH - 2, SCROLL_BAR_WIDTH, 
                                    _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        }

        if (isOverDown && m_scrollOffset < m_maxScrollOffset)
        {
            _GGL.gray.drawFillFrame(scrollBarX + 1, downArrowY1 + 1, SCROLL_BAR_WIDTH - 2, SCROLL_BAR_WIDTH, 
                                    _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            // Рисуем стрелку вниз (треугольник)
            int cx = scrollBarX + SCROLL_BAR_WIDTH / 2;
            int cy = downArrowY1 + SCROLL_BAR_WIDTH / 2;
            for (int i = SCROLL_BAR_WIDTH / 2 - 2; i >= 0; i--)
            {
                int x1 = cx - i;
                int x2 = cx + i;
                if (i > 0)
                {
                    _GGL.gray.drawPixel(x1, cy + i, _GGL.gray.WHITE);
                    _GGL.gray.drawPixel(x2, cy + i, _GGL.gray.WHITE);
                }
                _GGL.gray.drawPixel(cx, cy + SCROLL_BAR_WIDTH / 2 - 1, _GGL.gray.WHITE);
            }
        }
        else if (isOverDown)
        {
            _GGL.gray.drawFillFrame(scrollBarX + 1, downArrowY1 + 1, SCROLL_BAR_WIDTH - 2, SCROLL_BAR_WIDTH, 
                                    _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        }

        // Обработка нажатия ENTER для прокрутки
        if (_JOY.pressKeyENTER() && canScroll)
        {
            if (isOverUp && m_scrollOffset > 0)
            {
                m_scrollOffset--;
                m_lastScrollTime = currentTime;
                // Перерисовываем
                drawTextWithScroll();
                drawScrollBar(scrollBarX, scrollBarY, scrollBarH);
            }
            else if (isOverDown && m_scrollOffset < m_maxScrollOffset)
            {
                m_scrollOffset++;
                m_lastScrollTime = currentTime;
                // Перерисовываем
                drawTextWithScroll();
                drawScrollBar(scrollBarX, scrollBarY, scrollBarH);
            }
        }

        // Удержание для непрерывной прокрутки
        if ((Events.getEvent(PIN_BUTTON_ENTER) == SimpleEventType::LONG_PRESS) && canScroll)
        {
            // Если кнопка не нажата, но курсор всё ещё на стрелке - продолжаем прокрутку
            if (isOverUp && m_scrollOffset > 0 && _JOY.posX0 > 0)
            {
                // Используем флаг для непрерывной прокрутки
                if (m_isScrollingUp)
                {
                    m_scrollOffset--;
                    m_lastScrollTime = currentTime;
                    drawTextWithScroll();
                    drawScrollBar(scrollBarX, scrollBarY, scrollBarH);
                }
                m_isScrollingUp = true;
            }
            else
            {
                m_isScrollingUp = false;
            }

            if (isOverDown && m_scrollOffset < m_maxScrollOffset && _JOY.posX0 > 0)
            {
                if (m_isScrollingDown)
                {
                    m_scrollOffset++;
                    m_lastScrollTime = currentTime;
                    drawTextWithScroll();
                    drawScrollBar(scrollBarX, scrollBarY, scrollBarH);
                }
                m_isScrollingDown = true;
            }
            else
            {
                m_isScrollingDown = false;
            }
        }
    }
    else
    {
        m_isScrollingUp = false;
        m_isScrollingDown = false;
    }
}

void eTextScrollBox::calculateMaxScroll()
{
    short border{2};  // Уменьшенный отступ
    short charH{10}, charW{5};
    int contentW = m_sizeW - SCROLL_BAR_WIDTH - border - border;
    int charsPerLine = contentW / charW;
    
    if (charsPerLine <= 0) charsPerLine = 1;
    
    // Разбиваем текст на строки
    int lines = 0;
    int pos = 0;
    int len = m_text.length();
    
    while (pos < len)
    {
        lines++;
        // Ищем перенос строки или конец строки по ширине
        int end = pos + charsPerLine;
        if (end > len) end = len;
        
        // Ищем следующий символ новой строки
        int newlinePos = m_text.indexOf('\n', pos);
        if (newlinePos != -1 && newlinePos < end)
        {
            end = newlinePos + 1;
        }
        
        pos = end;
        // Если перенос строки, пропускаем его
        if (pos < len && m_text[pos - 1] == '\n')
        {
            // уже учли
        }
    }
    
    int linesVisible = (m_sizeH - border - border) / charH;
    if (linesVisible <= 0) linesVisible = 1;
    
    m_maxScrollOffset = lines - linesVisible;
    if (m_maxScrollOffset < 0) m_maxScrollOffset = 0;
    
    if (m_scrollOffset > m_maxScrollOffset)
        m_scrollOffset = m_maxScrollOffset;
}

void eTextScrollBox::drawTextWithScroll()
{
    short border{0};  // Уменьшенный отступ
    short charH{10}, charW{5};
    int contentW = m_sizeW - SCROLL_BAR_WIDTH - border - border;
    int contentH = m_sizeH - border - border;
    int charsPerLine = contentW / charW;
    int linesVisible = contentH / charH;
    
    if (charsPerLine <= 0) charsPerLine = 1;
    if (linesVisible <= 0) linesVisible = 1;
    
    // Заливаем область для текста белым
    _GGL.gray.drawFillFrame(xForm + border, yForm + border, contentW, contentH, 
                            _GGL.gray.BLACK, _GGL.gray.WHITE);
    
    // Разбиваем текст на строки
    std::vector<String> lines;
    int pos = 0;
    int len = m_text.length();
    
    while (pos < len)
    {
        int end = pos + charsPerLine;
        if (end > len) end = len;
        
        // Ищем перенос строки
        int newlinePos = m_text.indexOf('\n', pos);
        if (newlinePos != -1 && newlinePos < end)
        {
            end = newlinePos;
            lines.push_back(m_text.substring(pos, end));
            pos = newlinePos + 1;
            continue;
        }
        
        // Если есть слово, которое не влезает, обрезаем по слову
        String line = m_text.substring(pos, end);
        lines.push_back(line);
        pos = end;
    }
    
    // Рисуем строки с учетом прокрутки
    int startLine = m_scrollOffset;
    int endLine = startLine + linesVisible;
    if (endLine > (int)lines.size()) endLine = lines.size();
    
    int textColor = m_isActive ? _GGL.gray.BLACK : _GGL.gray.DARK_GRAY;
    
    for (int i = startLine; i < endLine; i++)
    {
        int yPos = yForm + border + (i - startLine) * charH;
        _GGL.gray.writeLine(xForm + border + 1, yPos + 1, lines[i], 10, 1, _GGL.gray.BLACK);
    }
}

void eTextScrollBox::drawScrollBar(int scrollX, int scrollY, int scrollH)
{
    // Рисуем фон скролл-бара (светло-серый)
    _GGL.gray.drawFillFrame(scrollX + 1, scrollY, SCROLL_BAR_WIDTH + 1, scrollH, 
                            _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
    
    if (m_maxScrollOffset <= 0)
    {
        // Если текст помещается, просто показываем пустой скролл-бар
        return;
    }
    
    // Вычисляем позицию ползунка
    float ratio = (float)m_scrollOffset / m_maxScrollOffset;
    int thumbHeight = scrollH / (m_maxScrollOffset + 2);
    if (thumbHeight < 5) thumbHeight = 5;
    
    int thumbY = scrollY + ratio * (scrollH - thumbHeight);
    if (thumbY < scrollY) thumbY = scrollY;
    if (thumbY + thumbHeight > scrollY + scrollH) thumbY = scrollY + scrollH - thumbHeight;
    
    // Рисуем ползунок
    // _GGL.gray.drawFillFrame(scrollX + 1, thumbY, SCROLL_BAR_WIDTH, thumbHeight, 
    //                         _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
    _GGL.gray.drawFrame(scrollX, thumbY, SCROLL_BAR_WIDTH, thumbHeight, 
                        _GGL.gray.BLACK);
    
    // Рисуем верхнюю стрелку (треугольник)
    int cx = scrollX + SCROLL_BAR_WIDTH / 2;
    int arrowY = scrollY + 2;
    for (int i = 0; i < 3; i++)
    {
        for (int x = cx - i; x <= cx + i; x++)
        {
            _GGL.gray.drawPixel(x, arrowY + i, _GGL.gray.BLACK);
        }
    }
    
    // Рисуем нижнюю стрелку (треугольник)
    arrowY = scrollY + scrollH - 4;
    for (int i = 0; i < 3; i++)
    {
        for (int x = cx - (2 - i); x <= cx + (2 - i); x++)
        {
            _GGL.gray.drawPixel(x, arrowY + i, _GGL.gray.BLACK);
        }
    }
}

bool eTextScrollBox::isPointInRect(int px, int py, int rx, int ry, int rw, int rh) const
{
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
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
            x = xForm;
            y += lii;
        }
    }
}

/* eLink */
void eLink::show()
{
    uint8_t sizeText = m_text.length();
    uint8_t yy{}, chi{5}, lii{8};
    int x{xForm + 1}, y{yForm};

    if (!m_isActive)
    {
        for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.DARK_GRAY);
            if (m_text[i] == '\n')
            {
                yy += lii;
                xx = -chi;
            }
        }
        return;
    }

    if ((_JOY.posX0 >= x && _JOY.posX0 <= (x + (sizeText * chi))) &&
        (_JOY.posY0 >= y - 2 && _JOY.posY0 <= y + lii + 2))
    {
        _GGL.gray.drawBox(x - 1, y, (sizeText * chi) + 2, lii + 1, _GGL.gray.BLACK);

        for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.WHITE);
            if (m_text[i] == '\n')
            {
                yy += lii;
                xx = -chi;
            }
        }

        if (g_activeElement == nullptr || g_activeElement == this)
        {
            if (_JOY.pressKeyENTER() == true)
            {
                for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
                {
                    _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.DARK_GRAY);
                    if (m_text[i] == '\n')
                    {
                        yy += lii;
                        xx = -chi;
                    }
                }
                m_onClick();
                if (g_activeElement == this)
                {
                    g_activeElement = nullptr;
                }
            }
        }
    }
    else
    {
        for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
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

/* eLinkLabel */
void eLinkLabel::show()
{
    uint8_t sizeText = m_text.length();
    uint8_t yy{}, chi{5}, lii{8};
    int x{xForm + 1}, y{yForm};

    if (!m_isActive)
    {
        for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.DARK_GRAY);
            if (m_text[i] == '\n')
            {
                yy += lii;
                xx = -chi;
            }
        }
        return;
    }

    if ((_JOY.posX0 >= x && _JOY.posX0 <= (x + (sizeText * chi))) &&
        (_JOY.posY0 >= y - 2 && _JOY.posY0 <= y + lii + 2))
    {
        _GGL.gray.drawBox(x - 1, y, (sizeText * chi) + 2, lii + 1, _GGL.gray.BLACK);

        for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
        {
            _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.WHITE);
            if (m_text[i] == '\n')
            {
                yy += lii;
                xx = -chi;
            }
        }

        if (g_activeElement == nullptr || g_activeElement == this)
        {
            if (_JOY.pressKeyENTER() == true)
            {
                for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
                {
                    _GGL.gray.writeChar(xx + x, yy + y, m_text[i], 10, 1, _GGL.gray.DARK_GRAY);
                    if (m_text[i] == '\n')
                    {
                        yy += lii;
                        xx = -chi;
                    }
                }
                m_onClick();
                if (g_activeElement == this)
                {
                    g_activeElement = nullptr;
                }
            }
        }
    }
    else
    {
        for (int i = 0, xx = 0; i < sizeText && xx < (sizeText * chi); i++, xx += chi)
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
    _GGL.gray.drawHLine(xForm, yForm, wForm, _GGL.gray.BLACK, 1);
}

/* eCheckbox */
void eCheckbox::show()
{
    if (!m_isActive)
    {
        _GGL.gray.drawFillFrame(xForm, yForm, 10, 10, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        _GGL.gray.writeLine(xForm + 15, yForm, m_text, 10, 1, _GGL.gray.DARK_GRAY);
        return;
    }

    if (m_checked == true)
    {
        _GGL.gray.drawFillFrame(xForm, yForm, 10, 10, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
    }
    if (m_checked == false)
    {
        _GGL.gray.drawFillFrame(xForm, yForm, 10, 10, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
    }

    _GGL.gray.writeLine(xForm + 15, yForm, m_text, 10, 1, _GGL.gray.BLACK);

    if ((_JOY.posX0 >= xForm) && (_JOY.posX0 <= xForm + 10) &&
        ((_JOY.posY0 >= yForm) && (_JOY.posY0 <= yForm + 10)))
    {
        _GGL.gray.drawBox(xForm, yForm, 10, 10, _GGL.gray.BLACK);

        if (g_activeElement == nullptr || g_activeElement == this)
        {
            if (_JOY.pressKeyENTER() == true)
            {
                m_checked = !m_checked;
                delay(250);
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
    for (int y = yForm; y < 148; y += m_h)
    {
        for (int x = xForm; x < 256; x += m_w)
        {
            _GGL.gray.bitmap(x, y, m_bitmap, m_w, m_h, _GGL.gray.TRANSPARENT);
        }
    }
}

/* Plotter */
/* ePlotter implementation */
void ePlotter::show()
{
    // Очищаем область
    _GGL.gray.drawFillFrame((short)xForm, (short)yForm, (short)m_width, (short)m_height, GRAY::BLACK, GRAY::WHITE);
    
    // Рисуем фрейм
    drawFrame();
    
    // Если нет данных - выводим сообщение
    if (m_data.empty())
    {
        String msg = "No data";
        int msgX = xForm + (m_width - msg.length() * 5) / 2;
        int msgY = yForm + (m_height - 8) / 2;
        _GGL.gray.writeLine((short)msgX, (short)msgY, msg, 10, 1, GRAY::DARK_GRAY);
        return;
    }
    
    // Рисуем сетку
    drawGrid();
    
    // Рисуем данные
    drawData();
    
    // Подписи осей
    drawAxisLabels();
}

void ePlotter::drawFrame()
{
    // Внешняя рамка
    // _GGL.gray.drawFrame((short)xForm, (short)yForm, (short)m_width, (short)m_height, GRAY::BLACK);
    
    // Внутренняя рамка (область графика)
    int margin = 0;
    int plotX = xForm + margin;
    int plotY = yForm;
    int plotW = m_width - margin * 2;
    int plotH = m_height;
    
    if (plotW > 10 && plotH > 10)
    {
        _GGL.gray.drawFrame((short)plotX, (short)plotY, (short)plotW, (short)plotH, GRAY::BLACK);
        // _GGL.gray.drawFrame((short)xForm, (short)yForm, (short)m_width, (short)m_height, GRAY::BLACK);
    }
}

void ePlotter::drawGrid()
{
    int margin = 0;
    int plotX = xForm + margin;
    int plotY = yForm;
    int plotW = m_width - margin * 2;
    int plotH = m_height;
    
    if (plotW < 10 || plotH < 10)
        return;
    
    // Горизонтальные линии сетки (4 линии)
    for (int i = 1; i < 5; i++)
    {
        int y = plotY + (plotH * i) / 5;
        _GGL.gray.drawHLine((short)plotX, (short)y, (short)plotW, GRAY::LIGHT_GRAY, 1);
    }
    
    // Вертикальные линии сетки (6 линий)
    for (int i = 1; i < 7; i++)
    {
        int x = plotX + (plotW * i) / 7;
        _GGL.gray.drawVLine((short)x, (short)plotY, (short)plotH, GRAY::LIGHT_GRAY, 1);
    }
}

void ePlotter::drawData()
{
    int margin = 0;
    int plotX = xForm + margin;
    int plotY = yForm;
    int plotW = m_width - margin * 2;
    int plotH = m_height;
    
    if (plotW < 10 || plotH < 10 || m_data.size() < 2)
        return;
    
    float range = m_maxValue - m_minValue;
    if (range < 0.001f) range = 1.0f;
    
    size_t dataSize = m_data.size();
    
    // Рисуем линию графика
    for (size_t i = 0; i < dataSize - 1; i++)
    {
        float v1 = m_data[i].value;
        float v2 = m_data[i + 1].value;
        
        int x1 = plotX + (plotW * i) / (dataSize - 1);
        int x2 = plotX + (plotW * (i + 1)) / (dataSize - 1);
        
        int y1 = plotY + plotH - (int)(((v1 - m_minValue) / range) * plotH);
        int y2 = plotY + plotH - (int)(((v2 - m_minValue) / range) * plotH);
        
        // Ограничиваем координаты
        if (y1 < plotY) y1 = plotY;
        if (y1 > plotY + plotH) y1 = plotY + plotH;
        if (y2 < plotY) y2 = plotY;
        if (y2 > plotY + plotH) y2 = plotY + plotH;
        
        _GGL.gray.drawLine((short)x1, (short)y1, (short)x2, (short)y2, m_lineColor);
    }
    
    // Рисуем последнюю точку
    if (!m_data.empty())
    {
        float v = m_data.back().value;
        int x = plotX + plotW;
        int y = plotY + plotH - (int)(((v - m_minValue) / range) * plotH);
        if (y >= plotY && y <= plotY + plotH)
        {
            _GGL.gray.drawFillCircle((short)x, (short)y, 3, m_lineColor, m_lineColor);
        }
    }
}

void ePlotter::drawAxisLabels()
{
    int margin = 0;
    int plotX = xForm + margin;
    int plotY = yForm;
    int plotW = m_width - margin * 2;
    int plotH = m_height;
    
    if (plotW < 30 || plotH < 20)
        return;
    
    // Подписи по оси Y (мин и макс) - внутри графика слева
    String minLabel = String((int)m_minValue);
    String maxLabel = String((int)m_maxValue);
    
    // Максимальное значение - вверху слева
    _GGL.gray.writeLine((short)(plotX + 2), (short)(plotY + 2), maxLabel, 10, 1, GRAY::DARK_GRAY);
    
    // Минимальное значение - внизу слева
    _GGL.gray.writeLine((short)(plotX + 2), (short)(plotY + plotH - 10), minLabel, 10, 1, GRAY::DARK_GRAY);
    
    // Количество точек - справа внизу
    String countLabel = String(m_data.size());
    int countX = plotX + plotW - countLabel.length() * 5 - 2;
    _GGL.gray.writeLine((short)countX, (short)(plotY + plotH - 10), countLabel, 10, 1, GRAY::DARK_GRAY);
}

/* eKeyboard */
void eKeyboard::show()
{
    if (!m_active || !m_isActive)
        return;

    if (m_active && g_activeElement == nullptr)
    {
        g_activeElement = this;
    }

    // Выбираем текущие ряды в зависимости от режима
    const std::vector<String>& currentRow1 = m_isNumberMode ? row1Numbers : row1;
    const std::vector<String>& currentRow2 = m_isNumberMode ? row2Numbers : row2;
    const std::vector<String>& currentRow3 = m_isNumberMode ? row3Numbers : row3;

    int keySpacing = 2;
    int keyW = m_keyW;
    int keyH = m_keyH;
    int startX = xForm;
    int startY = yForm;

    // Рисуем фон клавиатуры
    int maxRowSize = max(currentRow1.size(), max(currentRow2.size(), currentRow3.size()));
    int totalWidth = maxRowSize * (keyW + keySpacing) + keySpacing;
    int totalHeight = 3 * (keyH + keySpacing) + keySpacing;
    _GGL.gray.drawFillFrame(0, startY, 256, totalHeight, _GGL.gray.LIGHT_GRAY, _GGL.gray.LIGHT_GRAY);

    int currentY = startY + keySpacing;

    // Ряд 1
    int row1Width = currentRow1.size() * (keyW + keySpacing) - keySpacing;
    int row1Offset = (totalWidth - row1Width) / 2;
    int currentX = startX + row1Offset;
    for (const auto &key : currentRow1)
    {
        String displayKey = key;
        // Для букв применяем CapsLock
        if (!m_isNumberMode && m_capsLock && key.length() == 1 && isalpha(key[0]))
            displayKey.toUpperCase();
        else if (!m_isNumberMode && !m_capsLock && key.length() == 1 && isalpha(key[0]))
            displayKey.toLowerCase();
        
        bool isHighlighted = isKeyPressed(currentX, currentY, keyW, keyH);
        drawKey(currentX, currentY, keyW, keyH, displayKey, isHighlighted);
        currentX += keyW + keySpacing;
    }

    // Ряд 2
    currentY += keyH + keySpacing;
    int row2Width = currentRow2.size() * (keyW + keySpacing) - keySpacing;
    int row2Offset = (totalWidth - row2Width) / 2;
    currentX = startX + row2Offset;
    for (const auto &key : currentRow2)
    {
        String displayKey = key;
        if (!m_isNumberMode && m_capsLock && key.length() == 1 && isalpha(key[0]))
            displayKey.toUpperCase();
        else if (!m_isNumberMode && !m_capsLock && key.length() == 1 && isalpha(key[0]))
            displayKey.toLowerCase();
        
        bool isHighlighted = isKeyPressed(currentX, currentY, keyW, keyH);
        drawKey(currentX, currentY, keyW, keyH, displayKey, isHighlighted);
        currentX += keyW + keySpacing;
    }

    // Ряд 3
    currentY += keyH + keySpacing;
    int row3Width = 0;
    for (const auto &key : currentRow3)
    {
        int w = keyW;
        if (key == " ")
            w = keyW * 2;
        row3Width += w + keySpacing;
    }
    row3Width -= keySpacing;
    int row3Offset = (totalWidth - row3Width) / 2;
    currentX = startX + row3Offset;
    for (const auto &key : currentRow3)
    {
        int w = keyW;
        if (key == " ")
            w = keyW * 2;

        String displayLabel = key;
        if (key == "CL")
        {
            displayLabel = m_capsLock ? "CL" : "cl";
        }
        else if (key == "12" || key == "AB")
        {
            // Отображаем текущий режим
            displayLabel = m_isNumberMode ? "AB" : "12";
        }
        else if (!m_isNumberMode && key != "BS" && !key.isEmpty() && key != " " && key != "CL" && key != "12")
        {
            if (m_capsLock)
                displayLabel.toUpperCase();
            else
                displayLabel.toLowerCase();
        }

        bool isHighlighted = isKeyPressed(currentX, currentY, w, keyH);
        drawKey(currentX, currentY, w, keyH, displayLabel, isHighlighted);
        currentX += w + keySpacing;
    }

    // Обработка нажатий
    unsigned long currentTime = millis();

    // Ряд 1
    currentY = startY + keySpacing;
    row1Width = currentRow1.size() * (keyW + keySpacing) - keySpacing;
    row1Offset = (totalWidth - row1Width) / 2;
    currentX = startX + row1Offset;
    for (const auto &key : currentRow1)
    {
        if (isKeyPressed(currentX, currentY, keyW, keyH) && _JOY.pressKeyENTER())
        {
            if (currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (m_onCharInput)
                {
                    char ch = key[0];
                    if (!m_isNumberMode)
                    {
                        if (m_capsLock)
                            ch = toupper(ch);
                        else
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

    // Ряд 2
    currentY += keyH + keySpacing;
    row2Width = currentRow2.size() * (keyW + keySpacing) - keySpacing;
    row2Offset = (totalWidth - row2Width) / 2;
    currentX = startX + row2Offset;
    for (const auto &key : currentRow2)
    {
        if (isKeyPressed(currentX, currentY, keyW, keyH) && _JOY.pressKeyENTER())
        {
            if (currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (m_onCharInput)
                {
                    char ch = key[0];
                    if (!m_isNumberMode)
                    {
                        if (m_capsLock)
                            ch = toupper(ch);
                        else
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

    // Ряд 3
    currentY += keyH + keySpacing;
    row3Width = 0;
    for (const auto &key : currentRow3)
    {
        int w = keyW;
        if (key == " ")
            w = keyW * 2;
        row3Width += w + keySpacing;
    }
    row3Width -= keySpacing;
    row3Offset = (totalWidth - row3Width) / 2;
    currentX = startX + row3Offset;
    for (const auto &key : currentRow3)
    {
        int w = keyW;
        if (key == " ")
            w = keyW * 2;

        if (isKeyPressed(currentX, currentY, w, keyH) && _JOY.pressKeyENTER())
        {
            if (currentTime - m_lastKeyPressTime >= m_keyRepeatDelay)
            {
                if (key == "BS")
                {
                    if (m_inputText.length() > 0)
                    {
                        m_inputText.remove(m_inputText.length() - 1);
                        if (m_onCharInput)
                            m_onCharInput('\b');
                    }
                }
                else if (key == "CL")
                {
                    m_capsLock = !m_capsLock;
                }
                else if (key == "12" || key == "AB")
                {
                    // Переключение между режимами
                    toggleNumberMode();
                }
                else if (!key.isEmpty() && key != " " && key != "BS" && key != "CL" && key != "12" && key != "AB")
                {
                    if (m_onCharInput)
                    {
                        char ch = key[0];
                        if (!m_isNumberMode)
                        {
                            if (m_capsLock)
                                ch = toupper(ch);
                            else
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

    if (!m_active && g_activeElement == this)
    {
        g_activeElement = nullptr;
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
    // Если есть глобально активный элемент и это не мы - рисуем в неактивном состоянии
    if (g_activeElement != nullptr && g_activeElement != this)
    {
        int inputX = xForm;
        int inputY = yForm;
        int inputW = m_width > 0 ? m_width : 200;
        int inputH = m_height > 0 ? m_height : 20;

        if (m_label.length() > 0)
        {
            _GGL.gray.writeLine(inputX, inputY - 12, m_label, 10, 1, _GGL.gray.DARK_GRAY);
        }

        _GGL.gray.drawFillFrame(inputX, inputY, inputW, inputH, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        _GGL.gray.drawFrame(inputX, inputY, inputW, inputH, _GGL.gray.DARK_GRAY);

        String displayText = m_text;
        if (displayText.length() > 0)
        {
            int maxChars = (inputW - 6) / 5;
            if ((int)displayText.length() > maxChars)
            {
                displayText = "..." + displayText.substring(displayText.length() - maxChars + 3);
            }
            _GGL.gray.writeLine(inputX + 3, inputY + 3, displayText, 10, 1, _GGL.gray.DARK_GRAY);
        }
        return;
    }

    unsigned long currentTime = millis();

    int inputX = xForm;
    int inputY = yForm;
    int inputW = m_width > 0 ? m_width : 200;
    int inputH = m_height > 0 ? m_height : 20;

    if (m_label.length() > 0)
    {
        _GGL.gray.writeLine(inputX, inputY - 12, m_label, 10, 1, _GGL.gray.BLACK);
    }

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

    // Проверяем, можно ли активировать этот элемент
    bool canActivate = (g_activeElement == nullptr || g_activeElement == this);
    
    // Проверяем наведение курсора на поле ввода для АКТИВАЦИИ (только если не в режиме редактирования)
    bool isCursorOverInputForActivation = (m_isEditing == false) && canActivate &&
                             isPointInRect(_JOY.posX0, _JOY.posY0, inputX, inputY, inputW, inputH);

    // ОТКРЫТИЕ клавиатуры при нажатии ENTER на поле ввода
    if (isCursorOverInputForActivation && _JOY.pressKeyENTER() && (currentTime - m_lastToggleTime >= TOGGLE_COOLDOWN))
    {
        // Если есть другой активный элемент - деактивируем его
        if (g_activeElement != nullptr && g_activeElement != this)
        {
            g_activeElement->deactivate();
        }
        
        m_isEditing = true;
        m_isActive = true;
        g_activeElement = this;
        if (m_keyboard)
        {
            // Позиционируем клавиатуру статично внизу экрана по центру
            int kbW, kbH;
            m_keyboard->getKeyboardSize(kbW, kbH, 18, 14);
            // Центрируем по горизонтали, размещаем внизу с отступом 2 пикселя
            int kbX = (256 - kbW) / 2;
            int kbY = 160 - kbH - 2;
            m_keyboard->setStaticPosition(kbX, kbY);
            
            m_keyboard->setActive(true);
            m_keyboard->setIsActive(true);
            m_keyboard->setText(m_text);
            ElementZOrderManager::bringToFront(m_keyboard);
        }
        ElementZOrderManager::bringToFront(this);
        m_lastToggleTime = currentTime;
        
        // ВАЖНО: выходим после открытия, чтобы не обрабатывать это же нажатие ENTER для закрытия
        return;
    }

    // ОТОБРАЖЕНИЕ клавиатуры и обработка ввода
    if (m_isEditing && m_keyboard)
    {
        // Отображаем клавиатуру
        m_keyboard->show();

        // Проверяем наведение курсора на поле ввода для ЗАКРЫТИЯ
        bool isCursorOverInputForClosing = isPointInRect(_JOY.posX0, _JOY.posY0, inputX, inputY, inputW, inputH);
        
        // ЗАКРЫТИЕ клавиатуры при нажатии ENTER на поле ввода
        if (isCursorOverInputForClosing && _JOY.pressKeyENTER() && (currentTime - m_lastToggleTime >= TOGGLE_COOLDOWN))
        {
            // Закрываем клавиатуру (пользователь закончил ввод)
            m_isEditing = false;
            m_isActive = false;
            if (g_activeElement == this)
                g_activeElement = nullptr;
            if (m_keyboard)
            {
                m_keyboard->setActive(false);
                m_keyboard->setIsActive(false);
                m_keyboard->clearStaticPosition();
            }
            m_lastToggleTime = currentTime;
            // Вызываем колбэк, если он задан
            if (m_onTextChanged)
                m_onTextChanged(m_text);
        }
    }
}

/* exForm show */
int exForm::showForm()
{
    Button closeFormBtn;
    Button minimizeFormBtn;

    int sizeStack = formsStack.size();

    switch (eFormShowMode)
    {
    case FULLSCREEN:
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x, element->m_y + 12, element->m_w, element->m_h);
        }
        break;
    case MAXIMIZED:
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x, element->m_y + 12, element->m_w, element->m_h);
        }
        break;
    case NORMAL:
        for (const auto &element : elements)
        {
            element->setPosition(element->m_x + 20, element->m_y + 26, element->m_w - 40, element->m_h);
        }
        break;
    }

    if (eFormShowMode == FULLSCREEN)
    {
        // if (closeFormBtn.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 243, 0, _JOY.posX0, _JOY.posY0))
        // {
        //     return 1;
        // }

        // if (minimizeFormBtn.button(_SICON.mini_13x13, _SICON.mini_13x13_w, _SICON.mini_13x13_h, 230, 0, _JOY.posX0, _JOY.posY0))
        // {
        //     // Сворачиваем форму
        //     ::minimizeForm(this);
        //     return 0; // Не закрываем, просто сворачиваем
        // }

        switch (eFormBackground)
        {
        case TRANSPARENT:
            // _GGL.gray.drawFrame(0, 12, 256, 148, _GGL.gray.BLACK);
            _GGL.gray.drawFrame(0, 0, 256, 160, _GGL.gray.BLACK);
            break;
        case WHITE:
            // _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.WHITE);
            _GGL.gray.drawFillFrame(0, 0, 256, 160, _GGL.gray.BLACK, _GGL.gray.WHITE);
            break;
        case LIGHT_GRAY:
            // _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
            _GGL.gray.drawFillFrame(0, 0, 256, 160, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
            break;
        case DARK_GRAY:
            // _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            _GGL.gray.drawFillFrame(0, 0, 256, 160, _GGL.gray.BLACK, _GGL.gray.DARK_GRAY);
            break;
        case BLACK:
            // _GGL.gray.drawFillFrame(0, 12, 256, 148, _GGL.gray.BLACK, _GGL.gray.BLACK);
            _GGL.gray.drawFillFrame(0, 0, 256, 160, _GGL.gray.BLACK, _GGL.gray.BLACK);
            break;
        default:
            // _GGL.gray.drawFrame(0, 12, 256, 148, _GGL.gray.BLACK);
            _GGL.gray.drawFrame(0, 0, 256, 160, _GGL.gray.BLACK);
            break;
        }

        // _GGL.gray.drawFillFrame(0, 0, 231, 13, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        // _GRF.print(title, 5, 2, 10, 5);

        // uint8_t xSizeStack{};
        // if (sizeStack <= 9)
        //     xSizeStack = 205;
        // if ((sizeStack >= 10) && (sizeStack <= 99))
        //     xSizeStack = 200;
        // _GRF.print((String)sizeStack, xSizeStack, 2, 10, 5);
    }

    if (eFormShowMode == MAXIMIZED)
    {
        if (closeFormBtn.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 243, 0, _JOY.posX0, _JOY.posY0))
        {
            return 1;
        }

        if (minimizeFormBtn.button(_SICON.mini_13x13, _SICON.mini_13x13_w, _SICON.mini_13x13_h, 230, 0, _JOY.posX0, _JOY.posY0))
        {
            ::minimizeForm(this);
            return 0;
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

        _GGL.gray.drawFillFrame(0, 0, 231, 13, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        _GRF.print(title, 5, 2, 10, 5);

        uint8_t xSizeStack{};
        if (sizeStack <= 9)
            xSizeStack = 205;
        if ((sizeStack >= 10) && (sizeStack <= 99))
            xSizeStack = 200;
        _GRF.print("[" + (String)sizeStack + "]", xSizeStack, 2, 10, 5);
    }

    if (eFormShowMode == NORMAL)
    {
        if (closeFormBtn.button(_SICON.close_13x13, _SICON.close_13x13_w, _SICON.close_13x13_h, 223, outerBoundaryForm - 12 + 6, _JOY.posX0, _JOY.posY0))
        {
            return 1;
        }

        if (minimizeFormBtn.button(_SICON.mini_13x13, _SICON.mini_13x13_w, _SICON.mini_13x13_h, 210, outerBoundaryForm - 12 + 6, _JOY.posX0, _JOY.posY0))
        {
            ::minimizeForm(this);
            return 0;
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

        _GGL.gray.drawFillFrame(outerBoundaryForm, outerBoundaryForm - 6, 191, 13, _GGL.gray.BLACK, _GGL.gray.LIGHT_GRAY);
        _GRF.print(title, outerBoundaryForm + 5, outerBoundaryForm - 4, 10, 5);

        uint8_t xSizeStack{};
        if (sizeStack <= 9)
            xSizeStack = 185;
        if ((sizeStack >= 10) && (sizeStack <= 99))
            xSizeStack = 180;
        _GRF.print((String)sizeStack, xSizeStack, outerBoundaryForm - 4, 10, 5);
    }

    // === СОРТИРОВКА ЭЛЕМЕНТОВ ПО Z-ПОРЯДКУ ===
    sortElementsByZOrder();

    // Выводим все элементы в отсортированном порядке
    for (auto element : elements)
    {
        if (element->isActiveElement())
        {
            bool canInteract = (g_activeElement == nullptr || g_activeElement == element);
            
            if (element->isInEditMode() && g_activeElement == element)
            {
                canInteract = true;
            }
            
            if (!canInteract)
            {
                element->setActive(false);
                element->show();
                element->setActive(true);
            }
            else
            {
                element->show();
            }
        }
        else
        {
            element->show();
        }
    }

    return 0;
}