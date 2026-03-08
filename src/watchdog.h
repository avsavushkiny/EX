#pragma once

#include <Arduino.h>
#include "task.h"
#include "taskDispatcher.h"

void taskWatchdogOnCore1(void *parameter)
{
    (void)parameter;

    const unsigned long TIMEOUT_MS = 500;

    while (true)
    {
        // Проверяем только если какая-то задача активна
        if (runningTaskInfo.isActive)
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
                // showError("Task timeout: " + runningTaskInfo.name);
                Serial.println("[WATCHDOG] Killed task: " + runningTaskInfo.name);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Проверяем каждые 100 мс
    }
}