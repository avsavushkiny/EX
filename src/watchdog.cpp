#pragma once

#include "Arduino.h"
#include "taskDispatcher.h"
#include "watchdog.h"
#include "esp_timer.h"
#include "ggl.h"

extern GGL _GGL;
extern TaskDispatcher _TD;

// Глобальная структура для отслеживания запущенной задачи
RunningTaskInfo runningTaskInfo = {"", 0, false};

// Вывод сообщения об ошибке
void showError(String message)
{
    _GGL.gray.clearBuffer();
    _GGL.gray.writeLine(10, 10, message, 10, 1, _GGL.gray.BLACK);
    _GGL.gray.sendBuffer();
}

// #ifndef WATCHDOG
// #else
// Задача watchdog для второго ядра (core 1)
void taskWatchdogOnCore1(void *parameter)
{
    (void)parameter;

    const unsigned long TIMEOUT_MS = 500;

    while (true)
    {
        // Проверяем только если какая-то задача активна
        if (runningTaskInfo.isActive && (runningTaskInfo.name != "energySave"))
        {
            unsigned long elapsed = millis() - runningTaskInfo.startTime;

            if (elapsed > TIMEOUT_MS)
            {
                // НАЙДЕНА ЗАВИСШАЯ ЗАДАЧА

                // Деактивируем в основном списке
                for (auto &task : tasks)
                {
                    if (task.name == runningTaskInfo.name)
                    {
                        task.activ = false;
                        break;
                    }
                }

                // Сбрасываем
                noInterrupts();
                runningTaskInfo.isActive = false;
                interrupts();

                // Выводим ошибку
                showError("ERROR Task timeout: " + runningTaskInfo.name);
                // Serial.println("[WATCHDOG] Killed task: " + runningTaskInfo.name);

                delay(5000);
                ESP.restart();
            }
        }

        Serial.println(runningTaskInfo.name);
        vTaskDelay(pdMS_TO_TICKS(100)); // Проверяем каждые 100 мс
    }
}
// #endif