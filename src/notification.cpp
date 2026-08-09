#include "notification.h"
#include "ex.h"

// Конструктор
Notification::Notification()
    : _isShown(false), _lastResult(NONE), _showTime(0), _timeout(0)
{
}

// Основной метод отображения уведомления
Notification::Result Notification::show(
    const String &title,
    const String &message,
    NotificationType type,
    ButtonType buttons,
    unsigned int timeout)
{
    _isShown = true;
    _lastResult = NONE;
    _timeout = timeout;
    _showTime = millis();

    int x, y, w, h;
    getWindowPosition(x, y, w, h);

    // Сохраняем содержимое экрана (если нужно)
    // Здесь можно добавить сохранение буфера для восстановления

    // Отрисовка уведомления
    drawShadow(x + 3, y + 3, w, h);
    drawWindow(x, y, w, h, type);
    drawTitle(x, y, w, title, type);
    drawMessage(x, y + TITLE_HEIGHT, w, h - TITLE_HEIGHT - BOTTOM_LINE_HEIGHT - BUTTON_HEIGHT - 10, message);
    drawBottomLine(x, y + h - BUTTON_HEIGHT - 8, w, type);

    // Обработка кнопок
    int selectedButton = 0;
    int numButtons = (buttons == OK_ONLY) ? 1 : 2;

    // Рисуем кнопки
    drawButtons(x, y + h - BUTTON_HEIGHT - 4, buttons, selectedButton);

    _GGL.gray.sendBuffer();

    // Обработка ввода
    Result result = handleInput(buttons);

    // Восстанавливаем экран (если нужно)
    // Здесь можно восстановить сохраненный буфер

    _isShown = false;
    _lastResult = result;

    return result;
}

// Простое уведомление
bool Notification::showSimple(
    const String &message,
    NotificationType type,
    unsigned int timeout)
{
    String title;
    switch (type)
    {
    case INFO:
        title = "Information";
        break;
    case SUCCESS:
        title = "Success";
        break;
    case WARNING:
        title = "Warning";
        break;
    case ERROR:
        title = "Error";
        break;
    }

    Result result = show(title, message, type, OK_ONLY, timeout);
    return (result == OK);
}

// Отрисовка тени
void Notification::drawShadow(int x, int y, int w, int h)
{
    // Тень справа
    _GGL.gray.drawBox(x + w - 4, y + 4, 4, h - 4, GRAY::DARK_GRAY);
    // Тень снизу
    _GGL.gray.drawBox(x + 4, y + h - 4, w - 4, 4, GRAY::DARK_GRAY);
}

// Отрисовка окна
void Notification::drawWindow(int x, int y, int w, int h, NotificationType type)
{
    // Основной фон
    _GGL.gray.drawFillFrame(x, y, w, h, GRAY::BLACK, GRAY::WHITE);

    // Цветная полоса сверху (как в Windows 10/11)
    GRAY::Color color = getTypeColor(type);
    _GGL.gray.drawBox(x + BORDER_SIZE, y + BORDER_SIZE, w - BORDER_SIZE * 2, 4, color);
}

// Отрисовка заголовка
void Notification::drawTitle(int x, int y, int w, const String &title, NotificationType type)
{
    GRAY::Color textColor = getTypeTextColor(type);

    // Иконка в зависимости от типа
    const char *icon = "";
    switch (type)
    {
    case INFO:
        icon = "INFO";
        break;
    case SUCCESS:
        icon = "SUCCESS";
        break;
    case WARNING:
        icon = "WARNING";
        break;
    case ERROR:
        icon = "ERROR";
        break;
    }

    // Заголовок с иконкой
    String fullTitle = String(icon) + " " + title;
    _GGL.gray.writeLine(x + 10, y + 8, fullTitle, 10, 1, textColor);
}

// Отрисовка сообщения
void Notification::drawMessage(int x, int y, int w, int h, const String &message)
{
    // Разбиваем сообщение на строки
    int maxCharsPerLine = (w - 20) / 5; // 5 - ширина символа для шрифта 10

    String line = "";
    int lineNum = 0;
    int currentPos = 0;

    while (currentPos < message.length() && lineNum < (h / 10))
    {
        line = "";
        int startPos = currentPos;

        // Проверяем, есть ли перенос строки в сообщении
        int newlinePos = message.indexOf('\n', currentPos);
        if (newlinePos != -1 && newlinePos - currentPos <= maxCharsPerLine)
        {
            line = message.substring(currentPos, newlinePos);
            currentPos = newlinePos + 1;
        }
        else
        {
            // Проверяем, не превышает ли строка максимальную длину
            int endPos = currentPos + maxCharsPerLine;
            if (endPos >= message.length())
            {
                line = message.substring(currentPos);
                currentPos = message.length();
            }
            else
            {
                // Ищем пробел для переноса
                int spacePos = message.lastIndexOf(' ', endPos);
                if (spacePos > currentPos)
                {
                    line = message.substring(currentPos, spacePos);
                    currentPos = spacePos + 1;
                }
                else
                {
                    line = message.substring(currentPos, endPos);
                    currentPos = endPos;
                }
            }
        }

        if (line.length() > 0)
        {
            _GGL.gray.writeLine(x + 10, y + 2 + lineNum * 10, line, 10, 1, GRAY::BLACK);
            lineNum++;
        }
    }
}

// Отрисовка нижней линии
void Notification::drawBottomLine(int x, int y, int w, NotificationType type)
{
    GRAY::Color color = getTypeColor(type);
    _GGL.gray.drawHLine(x + BORDER_SIZE, y, w - BORDER_SIZE * 2, color, 2);
}

// Отрисовка кнопок
void Notification::drawButtons(int x, int y, ButtonType buttons, int selectedButton)
{
    int buttonWidth = BUTTON_WIDTH;
    int totalWidth = 0;
    String btn1Text, btn2Text;

    switch (buttons)
    {
    case OK_ONLY:
        btn1Text = "OK";
        totalWidth = buttonWidth;
        break;
    case OK_CANCEL:
        btn1Text = "OK";
        btn2Text = "CANCEL";
        totalWidth = buttonWidth * 2 + BUTTON_GAP;
        break;
    case YES_NO:
        btn1Text = "YES";
        btn2Text = "NO";
        totalWidth = buttonWidth * 2 + BUTTON_GAP;
        break;
    }

    int startX = x + (220 - totalWidth) / 2;
    int btnY = y;

    // Кнопка 1
    drawButton(startX, btnY, buttonWidth, BUTTON_HEIGHT, btn1Text, selectedButton == 0);

    // Кнопка 2 (если есть)
    if (buttons != OK_ONLY)
    {
        drawButton(startX + buttonWidth + BUTTON_GAP, btnY, buttonWidth, BUTTON_HEIGHT, btn2Text, selectedButton == 1);
    }
}

// Отрисовка отдельной кнопки
void Notification::drawButton(int x, int y, int w, int h, const String &text, bool selected)
{
    if (selected)
    {
        // Выбранная кнопка
        _GGL.gray.drawFillFrame(x, y, w, h, GRAY::BLACK, GRAY::DARK_GRAY);
        _GGL.gray.writeLine(x + (w - text.length() * 5) / 2, y - 1 + 4, text, 10, 1, GRAY::WHITE);
    }
    else
    {
        // Обычная кнопка
        _GGL.gray.drawFillFrame(x, y, w, h, GRAY::BLACK, GRAY::WHITE);
        _GGL.gray.writeLine(x + (w - text.length() * 5) / 2, y - 1 + 4, text, 10, 1, GRAY::BLACK);
    }
}

// Обработка ввода
Notification::Result Notification::handleInput(ButtonType buttons)
{
    int selectedButton = 0;
    int maxButtons = (buttons == OK_ONLY) ? 1 : 2;
    unsigned long lastInputTime = 0;

    while (true)
    {
        // Проверка таймаута
        if (_timeout > 0 && (millis() - _showTime) > _timeout)
        {
            return (buttons == OK_ONLY) ? OK : CANCEL;
        }

        // Обновление FPS
        _GGL.gray.sendBuffer();

        // Обработка навигации по кнопкам
        if (_JOY.pressKeyA() || _JOY.pressKeyB())
        {
            if (millis() - lastInputTime > 200) // Дебаунс
            {
                selectedButton = (selectedButton + 1) % maxButtons;
                lastInputTime = millis();

                // Перерисовываем кнопки
                int x, y, w, h;
                getWindowPosition(x, y, w, h);
                drawButtons(x, y + h - BUTTON_HEIGHT - 4, buttons, selectedButton);
            }
        }

        // Обработка выбора
        if (_JOY.pressKeyENTER())
        {
            switch (buttons)
            {
            case OK_ONLY:
                return OK;
            case OK_CANCEL:
                return (selectedButton == 0) ? OK : CANCEL;
            case YES_NO:
                return (selectedButton == 0) ? YES : NO;
            }
        }

        // ESC - отмена
        if (_JOY.pressKeyEX())
        {
            switch (buttons)
            {
            case OK_ONLY:
                return OK;
            case OK_CANCEL:
                return CANCEL;
            case YES_NO:
                return NO;
            }
        }

        delay(10);
    }
}

// Получение позиции окна (центрирование)
void Notification::getWindowPosition(int &x, int &y, int &w, int &h)
{
    w = WINDOW_WIDTH;
    h = WINDOW_HEIGHT;
    x = (_WIDTH - w) / 2;
    y = (_HEIGHT - h) / 2;
}

// Получение цвета для типа уведомления
GRAY::Color Notification::getTypeColor(NotificationType type)
{
    switch (type)
    {
    case INFO:
        return GRAY::DARK_GRAY;
    case SUCCESS:
        return GRAY::DARK_GRAY; // Будет использоваться для нижней линии
    case WARNING:
        return GRAY::DARK_GRAY;
    case ERROR:
        return GRAY::DARK_GRAY;
    default:
        return GRAY::BLACK;
    }
}

// Получение цвета текста для типа уведомления
GRAY::Color Notification::getTypeTextColor(NotificationType type)
{
    switch (type)
    {
    case INFO:
        return GRAY::DARK_GRAY;
    case SUCCESS:
        return GRAY::DARK_GRAY;
    case WARNING:
        return GRAY::DARK_GRAY;
    case ERROR:
        return GRAY::DARK_GRAY;
    default:
        return GRAY::BLACK;
    }
}