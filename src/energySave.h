#pragma once

#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// Предварительные объявления (forward declarations)
class TaskDispatcher;
class GGL;
class Joystick;
class InstantMessage;

// Внешние объявления (должны быть определены в main)
extern TaskDispatcher _TD;
extern GGL _GGL;
extern Joystick _JOY;

// Глобальные переменные для аппаратного таймера
extern esp_timer_handle_t sleep_timer;
extern volatile bool sleep_timeout;
extern const uint64_t SLEEP_TIMEOUT_US;

// Callback функция таймера
void sleep_timer_callback(void *arg);

// Инициализация аппаратного таймера
void initSleepTimer();

// Запуск/перезапуск таймера
void resetSleepTimer();

// Остановка таймера
void stopSleepTimer();

// Освобождение ресурсов таймера
void deleteSleepTimer();

// Настройка GPIO для пробуждения
void setupWakeupGPIO();

// Проверка взаимодействия с джойстиком/кнопками
bool isTouched();

// Основная функция энергосбережения
void energySave();

// Функция для принудительной активации сна
void forceSleep();

// Инициализация задачи сна
void initSleepTimerTask();