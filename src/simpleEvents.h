#pragma once

#include <Arduino.h>
#include <map>

// Типы событий
enum class SimpleEventType
{
    NONE,         // Нет события
    SINGLE_CLICK, // Одинарный клик
    DOUBLE_CLICK, // Двойной клик
    LONG_PRESS    // Долгое нажатие
};

// Режим подключения кнопки
enum class ButtonMode
{
    PULL_UP,     // Кнопка на GND (обычное)
    PULL_DOWN,   // Кнопка на VCC (инверсное)
    AUTO         // Автоопределение
};

class SimpleEventSystem
{
private:
    struct ButtonState
    {
        int pin;
        ButtonMode mode;
        bool lastState;
        bool isPressed;
        bool buttonPressed;
        bool longPressTriggered;
        bool hasEvent;
        bool waitingForSecond;
        unsigned long pressStartTime;
        unsigned long lastDebounceTime;
        unsigned long lastPressTime;
        unsigned long lastClickTime;
        int clickCount;
        SimpleEventType lastEvent;
        bool modeDetected;  // Флаг, что режим определен

        ButtonState(int p = -1, ButtonMode m = ButtonMode::AUTO) 
            : pin(p), mode(m), lastState(HIGH), isPressed(false),
              buttonPressed(false), longPressTriggered(false), hasEvent(false),
              waitingForSecond(false), pressStartTime(0), lastDebounceTime(0),
              lastPressTime(0), lastClickTime(0), clickCount(0),
              lastEvent(SimpleEventType::NONE), modeDetected(false) {}
    };

    std::map<int, ButtonState> buttons;

    // Автоопределение режима кнопки
    ButtonMode detectButtonMode(int pin)
    {
        // Устанавливаем пин в INPUT с подтяжкой
        pinMode(pin, INPUT_PULLUP);
        delay(10);
        int readingPullUp = digitalRead(pin);
        
        // Устанавливаем пин в INPUT с подтяжкой к GND
        pinMode(pin, INPUT_PULLDOWN);
        delay(10);
        int readingPullDown = digitalRead(pin);
        
        // Возвращаем в INPUT_PULLUP (по умолчанию)
        pinMode(pin, INPUT_PULLUP);
        
        // Логика определения:
        // Если при PULLUP читаем HIGH, а при PULLDOWN читаем LOW - 
        // значит кнопка не нажата, но мы не знаем тип подключения
        // Определяем по разнице: если при PULLUP читаем HIGH - скорее всего кнопка на GND (PULLUP)
        // Если при PULLUP читаем LOW - скорее всего кнопка на VCC (PULLDOWN)
        
        if (readingPullUp == HIGH && readingPullDown == LOW) {
            // Не нажата - определяем по умолчанию PULL_UP
            return ButtonMode::PULL_UP;
        } else if (readingPullUp == LOW && readingPullDown == HIGH) {
            // Странная ситуация, скорее всего нажата
            return ButtonMode::PULL_UP;
        } else if (readingPullUp == LOW && readingPullDown == LOW) {
            // Нажата (оба показывают LOW) - не можем определить
            return ButtonMode::PULL_UP;  // По умолчанию
        } else {
            return ButtonMode::PULL_UP;  // По умолчанию
        }
    }

    // Более точное определение режима
    ButtonMode detectButtonModePrecise(int pin)
    {
        // Сначала настраиваем как INPUT (без подтяжки)
        pinMode(pin, INPUT);
        delay(10);
        int readingNoPull = digitalRead(pin);
        
        // Настраиваем с подтяжкой к VCC
        pinMode(pin, INPUT_PULLUP);
        delay(10);
        int readingPullUp = digitalRead(pin);
        
        // Настраиваем с подтяжкой к GND
        pinMode(pin, INPUT_PULLDOWN);
        delay(10);
        int readingPullDown = digitalRead(pin);
        
        // Возвращаем в INPUT_PULLUP
        pinMode(pin, INPUT_PULLUP);
        
        // Вывод отладочной информации
        Serial.printf("Pin %d: NoPull=%d, PullUp=%d, PullDown=%d\n", 
            pin, readingNoPull, readingPullUp, readingPullDown);
        
        // Логика определения:
        // 1. Если без подтяжки HIGH, с PULLUP HIGH, с PULLDOWN LOW -> кнопка на GND (PULL_UP)
        // 2. Если без подтяжки LOW, с PULLUP LOW, с PULLDOWN HIGH -> кнопка на VCC (PULL_DOWN)
        // 3. Если без подтяжки LOW, с PULLUP HIGH, с PULLDOWN LOW -> нажата кнопка на GND
        // 4. Если без подтяжки HIGH, с PULLUP LOW, с PULLDOWN HIGH -> нажата кнопка на VCC
        
        // Кнопка не нажата
        if (readingPullUp == HIGH && readingPullDown == LOW) {
            return ButtonMode::PULL_UP;
        }
        // Кнопка не нажата (другой вариант)
        else if (readingPullUp == HIGH && readingPullDown == HIGH) {
            // Невозможно определить, пробуем проверить снова через 100 мс
            delay(100);
            pinMode(pin, INPUT);
            delay(10);
            int readingNoPull2 = digitalRead(pin);
            
            if (readingNoPull2 == HIGH) {
                return ButtonMode::PULL_UP;
            } else {
                return ButtonMode::PULL_DOWN;
            }
        }
        // Кнопка нажата - определяем по значению без подтяжки
        else if (readingNoPull == LOW) {
            // Скорее всего кнопка на GND (PULL_UP)
            return ButtonMode::PULL_UP;
        } else {
            return ButtonMode::PULL_DOWN;
        }
    }

    bool isButtonPressed(bool reading, ButtonMode mode)
    {
        if (mode == ButtonMode::PULL_UP) {
            return reading == LOW;   // Кнопка на GND
        } else {
            return reading == HIGH;  // Кнопка на VCC
        }
    }

    void processButton(ButtonState &state)
    {
        unsigned long currentTime = millis();
        bool reading = digitalRead(state.pin);
        
        // Если режим не определен или AUTO - определяем
        if (state.mode == ButtonMode::AUTO || !state.modeDetected) {
            state.mode = detectButtonModePrecise(state.pin);
            state.modeDetected = true;
            Serial.printf("Pin %d mode detected: %s\n", state.pin,
                state.mode == ButtonMode::PULL_UP ? "PULL_UP" : "PULL_DOWN");
        }
        
        bool pressed = isButtonPressed(reading, state.mode);

        // Антидребезг
        if (reading != state.lastState)
        {
            state.lastDebounceTime = currentTime;
            state.lastState = reading;
            return;
        }

        if ((currentTime - state.lastDebounceTime) > 50)
        {
            // Обработка нажатия
            if (pressed && !state.buttonPressed)
            {
                state.buttonPressed = true;
                state.pressStartTime = currentTime;
                state.isPressed = true;
                
                if (state.waitingForSecond && (currentTime - state.lastPressTime) <= 300)
                {
                    state.clickCount++;
                    if (state.clickCount >= 2)
                    {
                        state.lastEvent = SimpleEventType::DOUBLE_CLICK;
                        state.hasEvent = true;
                        state.clickCount = 0;
                        state.waitingForSecond = false;
                        state.pressStartTime = 0;
                        state.longPressTriggered = false;
                    }
                }
                else
                {
                    state.clickCount = 1;
                    state.waitingForSecond = true;
                    state.lastPressTime = currentTime;
                    state.longPressTriggered = false;
                }
            }
            // Обработка отпускания
            else if (!pressed && state.buttonPressed)
            {
                state.buttonPressed = false;
                state.isPressed = false;
                unsigned long pressDuration = currentTime - state.pressStartTime;
                
                if (pressDuration >= 500)
                {
                    if (!state.longPressTriggered)
                    {
                        state.longPressTriggered = true;
                        state.lastEvent = SimpleEventType::LONG_PRESS;
                        state.hasEvent = true;
                        state.clickCount = 0;
                        state.waitingForSecond = false;
                    }
                }
                
                state.pressStartTime = 0;
            }
            // Обработка удержания
            else if (pressed && state.buttonPressed)
            {
                unsigned long pressDuration = currentTime - state.pressStartTime;
                
                if (pressDuration >= 500 && !state.longPressTriggered)
                {
                    state.longPressTriggered = true;
                    state.lastEvent = SimpleEventType::LONG_PRESS;
                    state.hasEvent = true;
                    state.clickCount = 0;
                    state.waitingForSecond = false;
                }
            }
            
            // Таймаут для одиночного клика
            if (state.waitingForSecond && state.clickCount == 1 && 
                (currentTime - state.lastPressTime) > 300)
            {
                state.lastEvent = SimpleEventType::SINGLE_CLICK;
                state.hasEvent = true;
                state.waitingForSecond = false;
                state.clickCount = 0;
                state.longPressTriggered = false;
            }
            
            // Сброс состояния после долгого нажатия
            if (state.longPressTriggered && !pressed)
            {
                state.longPressTriggered = false;
                state.clickCount = 0;
                state.waitingForSecond = false;
            }
        }
    }

public:
    // Добавить кнопку с автоопределением
    void addButton(int pin)
    {
        ButtonState state(pin, ButtonMode::AUTO);
        state.lastState = HIGH;
        pinMode(pin, INPUT_PULLUP);
        buttons[pin] = state;
    }

    // Добавить кнопку с указанием режима
    void addButton(int pin, ButtonMode mode)
    {
        ButtonState state(pin, mode);
        state.modeDetected = (mode != ButtonMode::AUTO);
        
        if (mode == ButtonMode::PULL_UP || mode == ButtonMode::AUTO) {
            state.lastState = HIGH;
            pinMode(pin, INPUT_PULLUP);
        } else {
            state.lastState = LOW;
            pinMode(pin, INPUT_PULLDOWN);
        }
        
        buttons[pin] = state;
    }

    // Принудительно определить режим для всех кнопок
    void detectAllModes()
    {
        for (auto &pair : buttons)
        {
            ButtonState &state = pair.second;
            if (state.mode == ButtonMode::AUTO || !state.modeDetected)
            {
                state.mode = detectButtonModePrecise(state.pin);
                state.modeDetected = true;
                
                // Настраиваем пин согласно определенному режиму
                if (state.mode == ButtonMode::PULL_UP) {
                    pinMode(state.pin, INPUT_PULLUP);
                    state.lastState = HIGH;
                } else {
                    pinMode(state.pin, INPUT_PULLDOWN);
                    state.lastState = LOW;
                }
                
                Serial.printf("Pin %d auto-detected as: %s\n", state.pin,
                    state.mode == ButtonMode::PULL_UP ? "PULL_UP" : "PULL_DOWN");
            }
        }
    }

    void update()
    {
        for (auto &pair : buttons)
        {
            processButton(pair.second);
        }
    }

    SimpleEventType getEvent(int pin)
    {
        auto it = buttons.find(pin);
        if (it != buttons.end())
        {
            SimpleEventType event = it->second.lastEvent;
            it->second.lastEvent = SimpleEventType::NONE;
            it->second.hasEvent = false;
            return event;
        }
        return SimpleEventType::NONE;
    }

    bool hasEvent(int pin)
    {
        auto it = buttons.find(pin);
        if (it != buttons.end())
        {
            return it->second.hasEvent;
        }
        return false;
    }

    bool isPressed(int pin)
    {
        auto it = buttons.find(pin);
        if (it != buttons.end())
        {
            return it->second.isPressed;
        }
        return false;
    }

    unsigned long getPressDuration(int pin)
    {
        auto it = buttons.find(pin);
        if (it != buttons.end() && it->second.isPressed)
        {
            return millis() - it->second.pressStartTime;
        }
        return 0;
    }

    // Получить определенный режим кнопки
    ButtonMode getButtonMode(int pin)
    {
        auto it = buttons.find(pin);
        if (it != buttons.end())
        {
            return it->second.mode;
        }
        return ButtonMode::AUTO;
    }
};

// Глобальный экземпляр
extern SimpleEventSystem Events;