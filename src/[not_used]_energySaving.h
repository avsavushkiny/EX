// [!] not used

#pragma once

#include <Arduino.h>
#include "taskDispatcher.h"
#include "ui.h"
#include "ggl.h"

#ifdef ESP32
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#endif

extern TaskDispatcher _TD;
extern Joystick _JOY;
extern GGL _GGL;

class EnergySaving
{
private:
    static unsigned long lastUserActivity;
    static bool energySavingMode;
    static const unsigned long INACTIVITY_TIMEOUT = 60000; // 60 секунд

public:
    // Инициализация системы энергосбережения
    static void begin()
    {
        lastUserActivity = millis();
        energySavingMode = false;
    }

    // Отслеживание активности пользователя
    static void trackUserActivity()
    {
        // Проверяем активность джойстика (движение или нажатия)
        if (_JOY.posX0 != _JOY.lastPosX0 || _JOY.posY0 != _JOY.lastPosY0 ||
            _JOY.pressKeyENTER() || _JOY.pressKeyEX())
        {

            lastUserActivity = millis();
            _JOY.lastPosX0 = _JOY.posX0;
            _JOY.lastPosY0 = _JOY.posY0;

            // Если был включен режим энергосбережения - выходим из него
            if (energySavingMode)
            {
                exitEnergySavingMode();
            }
        }

        // Проверяем таймаут неактивности
        if (!energySavingMode && (millis() - lastUserActivity > INACTIVITY_TIMEOUT))
        {
            enterEnergySavingMode();
        }
    }

    // Вход в режим энергосбережения
    static void enterEnergySavingMode()
    {
        energySavingMode = true;

        // Serial.println("Entering energy saving mode...");

        // Отключаем не критичные задачи
        _TD.removeTask("cursor");
        _TD.removeTask("monitor");
        _TD.removeTask("stackform");

        // Останавливаем аппаратный таймер для экономии энергии
        _TD.stopHardwareTimer();

        // Очищаем экран или выводим сообщение о энергосбережении
        _GGL.gray.clear();
        _GGL.gray.writeLine(50, 80, "Energy Saving Mode", 10, 1, _GGL.gray.BLACK);
        _GGL.gray.writeLine(70, 95, "Press any key to wake up", 8, 1, _GGL.gray.BLACK);

// Настраиваем ESP32 на низкое энергопотребление
#ifdef ESP32
        // Уменьшаем частоту процессора
        setCpuFrequencyMhz(80);

// Отключаем WiFi и Bluetooth если они были включены
// WiFi.mode(WIFI_OFF);
// btStop();
#endif

        Serial.println("Energy saving mode activated");
    }

    // Выход из режима энергосбережения
    static void exitEnergySavingMode()
    {
        energySavingMode = false;

        Serial.println("Exiting energy saving mode...");

// Восстанавливаем нормальную частоту процессора
#ifdef ESP32
        setCpuFrequencyMhz(240); // или ваша стандартная частота
#endif

        // Перезапускаем аппаратный таймер
        _TD.initHardwareTimer();

        // Восстанавливаем задачи
        _TD.runTask("cursor");
        _TD.runTask("monitor");
        _TD.runTask("stackform");

        // Очищаем экран
        _GGL.gray.clear();

        // Обновляем время активности
        lastUserActivity = millis();

        Serial.println("Energy saving mode deactivated");
    }

    // Функция для принудительной проверки активности (можно вызывать из других мест)
    static void resetInactivityTimer()
    {
        lastUserActivity = millis();
        if (energySavingMode)
        {
            exitEnergySavingMode();
        }
    }

    // Получение статуса режима энергосбережения
    static bool isEnergySavingMode()
    {
        return energySavingMode;
    }

    // Получение оставшегося времени до энергосбережения (в миллисекундах)
    static unsigned long getTimeToSleep()
    {
        if (energySavingMode)
            return 0;
        unsigned long elapsed = millis() - lastUserActivity;
        return (elapsed < INACTIVITY_TIMEOUT) ? (INACTIVITY_TIMEOUT - elapsed) : 0;
    }

    // Принудительное включение режима энергосбережения
    static void forceEnergySaving()
    {
        enterEnergySavingMode();
    }

    // Функция для задачи (wrapper)
    static void energySavingTask()
    {
        trackUserActivity();
    }
};

// Инициализация статических переменных
unsigned long EnergySaving::lastUserActivity = 0;
bool EnergySaving::energySavingMode = false;