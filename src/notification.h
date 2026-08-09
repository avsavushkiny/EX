/*
    [name]       Notification Library
    [chip]       ST75256 (display JLX256160-920)
    [resolution] 256x160
    [version]    0.1
    [create]     Alexander Savushkin
    [date]       09\08\2026
    [e-mail]     aeondc@gmail.com
    [github]     https://github.com/avsavushkiny/ggl
*/

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "ggl.h"
#include "input.h"
#include "graphics.h"

extern GGL _GGL;
extern Graphics _GRF;

class Notification
{
public:
    /* Тип уведомления */
    enum NotificationType
    {
        INFO,    // Информационное (синяя полоса)
        SUCCESS, // Успех (зеленая полоса)
        WARNING, // Предупреждение (желтая полоса)
        ERROR    // Ошибка (красная полоса)
    };

    /* Кнопки управления */
    enum ButtonType
    {
        OK_ONLY,   // Только кнопка "OK"
        OK_CANCEL, // Кнопки "OK" и "CANCEL"
        YES_NO     // Кнопки "YES" и "NO"
    };

    /* Результат нажатия */
    enum Result
    {
        NONE = 0,
        OK = 1,
        CANCEL = 2,
        YES = 3,
        NO = 4
    };

    /* Конструктор */
    Notification();

    /* Основной метод для отображения уведомления */
    Result show(
        const String &title,          // Заголовок уведомления
        const String &message,        // Текст сообщения
        NotificationType type = INFO, // Тип уведомления
        ButtonType buttons = OK_ONLY, // Тип кнопок
        unsigned int timeout = 0      // Таймаут авто-закрытия (0 - без таймаута)
    );

    /* Перегруженный метод для простых уведомлений */
    bool showSimple(
        const String &message,        // Текст сообщения
        NotificationType type = INFO, // Тип уведомления
        unsigned int timeout = 0      // Таймаут авто-закрытия
    );

    /* Метод для проверки состояния (был ли показан нотифай) */
    bool isShown() const { return _isShown; }

    /* Получить последний результат */
    Result getLastResult() const { return _lastResult; }

private:
    /* Внутренние константы */
    static constexpr int WINDOW_WIDTH = 220;     // Ширина окна
    static constexpr int WINDOW_HEIGHT = 100;    // Высота окна
    static constexpr int BORDER_SIZE = 2;        // Толщина рамки
    static constexpr int TITLE_HEIGHT = 20;      // Высота заголовка
    static constexpr int BOTTOM_LINE_HEIGHT = 2; // Толщина нижней линии
    static constexpr int BUTTON_HEIGHT = 16;     // Высота кнопки
    static constexpr int BUTTON_WIDTH = 50;      // Ширина кнопки
    static constexpr int BUTTON_GAP = 10;        // Расстояние между кнопками

    /* Состояние */
    bool _isShown;
    Result _lastResult;
    unsigned long _showTime;
    unsigned int _timeout;

    /* Вспомогательные методы */
    void drawWindow(int x, int y, int w, int h, NotificationType type);
    void drawTitle(int x, int y, int w, const String &title, NotificationType type);
    void drawMessage(int x, int y, int w, int h, const String &message);
    void drawBottomLine(int x, int y, int w, NotificationType type);
    void drawButtons(int x, int y, ButtonType buttons, int selectedButton);
    void drawShadow(int x, int y, int w, int h);

    /* Получение цвета для типа уведомления */
    GRAY::Color getTypeColor(NotificationType type);
    GRAY::Color getTypeTextColor(NotificationType type);

    /* Обработка ввода */
    Result handleInput(ButtonType buttons);
    void drawButton(int x, int y, int w, int h, const String &text, bool selected);

    /* Центрирование окна */
    void getWindowPosition(int &x, int &y, int &w, int &h);
};

#endif // NOTIFICATION_H

/*


// Пример 1: Простое уведомление
if (notif.showSimple("Operation completed successfully!", Notification::SUCCESS))
{
    // Пользователь нажал OK
    Serial.println("User confirmed notification");
}

// Пример 2: Уведомление с выбором
Notification::Result result = notif.show(
    "Confirm Action",
    "Do you really want to delete this file?",
    Notification::WARNING,
    Notification::YES_NO);

if (result == Notification::YES)
{
    // Выполнить действие
    Serial.println("Action confirmed");
}
else if (result == Notification::NO)
{
    Serial.println("Action cancelled");
}

// Пример 3: Уведомление с авто-закрытием
bool confirmed = notif.showSimple(
    "File saved successfully!",
    Notification::SUCCESS,
    2000 // Автоматически закроется через 2 секунды
);

// Пример 4: Информационное уведомление
result = notif.show(
    "System Information",
    "The system has been updated to the latest version.\nPlease restart the device.",
    Notification::INFO,
    Notification::OK_CANCEL);


 */