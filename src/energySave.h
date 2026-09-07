#pragma once

#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// Предварительные объявления
class TaskDispatcher;
class GGL;
class Joystick;
class InstantMessage;

// Внешние объявления
extern TaskDispatcher _TD;
extern GGL _GGL;
extern Joystick _JOY;

// Глобальные переменные
extern esp_timer_handle_t sleep_timer;
extern volatile bool sleep_timeout;
extern const uint64_t SLEEP_TIMEOUT_US;

// Функции управления сном
void sleep_timer_callback(void *arg);
void initSleepTimer();
void resetSleepTimer();
void stopSleepTimer();
void deleteSleepTimer();
void setupWakeupGPIO();

// Проверка активности
bool isTouched();
bool canEnterSleep();

// Основные функции
void energySave();
void forceSleep();
void initSleepTimerTask();

// Дополнительные функции управления
void enableEnergySave();
void disableEnergySave();
bool isEnergySaveEnabled();