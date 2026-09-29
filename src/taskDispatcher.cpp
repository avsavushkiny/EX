#pragma once

#include <algorithm>
#include <Arduino.h>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// --- Глобальные векторы ---
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;

static unsigned long taskStartTime = 0;
static String currentTaskName = "";

// --- Аппаратный таймер (1 мс) ---
static esp_timer_handle_t system_timer = nullptr;
static volatile unsigned long hardwareTicks = 0;
static const unsigned long TIMER_INTERVAL_US = 1000;

void system_timer_callback(void *arg)
{
    hardwareTicks++;
}

void TaskDispatcher::initHardwareTimer()
{
    if (system_timer != nullptr) return;

    esp_timer_create_args_t timer_args = {
        .callback = &system_timer_callback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "system_tick"};

    if (esp_timer_create(&timer_args, &system_timer) != ESP_OK) return;
    if (esp_timer_start_periodic(system_timer, TIMER_INTERVAL_US) != ESP_OK)
    {
        esp_timer_delete(system_timer);
        system_timer = nullptr;
    }
}

void TaskDispatcher::stopHardwareTimer()
{
    if (system_timer != nullptr)
        esp_timer_stop(system_timer);
}

unsigned long TaskDispatcher::getHardwareTicks()
{
    return hardwareTicks;
}

// --- Управление списком задач ---

int TaskDispatcher::sizeTasks()
{
    return tasks.size();
}

void TaskDispatcher::addTask(const TaskArguments &task)
{
    if (tasksMutex && xSemaphoreTake(tasksMutex, portMAX_DELAY) == pdTRUE)
    {
        tasks.push_back(task);
        xSemaphoreGive(tasksMutex);
    }
    else
    {
        tasks.push_back(task);
    }
}

bool TaskDispatcher::removeTaskVector(const String &taskName)
{
    bool ok = false;
    if (tasksMutex && xSemaphoreTake(tasksMutex, portMAX_DELAY) == pdTRUE)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [&taskName](const TaskArguments &t)
                               { return t.name == taskName; });
        if (it != tasks.end())
        {
            tasks.erase(it);
            ok = true;
        }
        xSemaphoreGive(tasksMutex);
    }
    return ok;
}

bool TaskDispatcher::removeTask(const String &taskName)
{
    for (auto &t : tasks)
    {
        if (t.activ && t.name == taskName)
        {
            t.activ = false;
            return true;
        }
    }
    return false;
}

bool TaskDispatcher::removeTaskIndex(const int index)
{
    for (auto &t : tasks)
    {
        if (t.activ && t.index == index)
        {
            t.activ = false;
            return true;
        }
    }
    return false;
}

bool TaskDispatcher::runTask(const String &taskName)
{
    for (auto &t : tasks)
    {
        if (!t.activ && t.name == taskName)
        {
            t.activ = true;
            return true;
        }
    }
    return false;
}

void TaskDispatcher::addTasksForSystems()
{
    for (TaskArguments &t : system0)
        tasks.push_back(t);
}

// --- Основной цикл воркера ---

void TaskDispatcher::workerCore0(void *arg)
{
    static_cast<TaskDispatcher *>(arg)->workerLoop(0);
}

void TaskDispatcher::workerCore1(void *arg)
{
    static_cast<TaskDispatcher *>(arg)->workerLoop(1);
}

void TaskDispatcher::workerLoop(int coreId)
{
    // Небольшая задержка, чтобы дать системе инициализироваться
    vTaskDelay(pdMS_TO_TICKS(50));

    for (;;)
    {
        if (!workersRunning)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        unsigned long currentRealTime   = millis();
        unsigned long currentHardwareTicks = getHardwareTicks();

        runTasksForCore(coreId, currentRealTime, currentHardwareTicks);

        // Отдаём управление FreeRTOS, чтобы не блокировать idle-таски
        // (в т.ч. watchdog). 1 тик ~1 мс.
        vTaskDelay(1);
    }
}

// --- Выполнение задач, привязанных к ядру ---

void TaskDispatcher::runTasksForCore(int coreId, unsigned long currentRealTime,
                                     unsigned long currentHardwareTicks)
{
    // Собираем ссылки на активные задачи, привязанные к этому ядру,
    // и сортируем по приоритету.
    std::vector<TaskArguments *> sortedTasks;
    sortedTasks.reserve(tasks.size());

    for (auto &task : tasks)
    {
        if (!task.activ || !task.f) continue;

        // Привязка к ядру
        bool belongsHere = false;
        if (task.core == CORE_ANY)
        {
            // По умолчанию все "any" — на CORE_1
            belongsHere = (coreId == 1);
        }
        else
        {
            belongsHere = (static_cast<int>(task.core) == coreId);
        }
        if (!belongsHere) continue;

        sortedTasks.push_back(&task);
    }

    std::sort(sortedTasks.begin(), sortedTasks.end(),
              [](const TaskArguments *a, const TaskArguments *b)
              {
                  return a->priority > b->priority;
              });

    for (auto taskPtr : sortedTasks)
    {
        auto &task = *taskPtr;

        // Проверка расписания
        bool shouldRun = false;
        if (task.interval > 0)
        {
            // Периодическая задача — реальное время (мс)
            shouldRun = (currentRealTime >= task.nextRunTime);
        }
        else
        {
            // Задача «каждый тик» — аппаратные тики
            shouldRun = (currentHardwareTicks >= task.nextRunTime);
        }

        if (!shouldRun) continue;

        unsigned long startTime = micros();

        // Отмечаем, что задача выполняется
        task.running = true;
        runningTaskInfo.name      = task.name;
        runningTaskInfo.startTime = millis();
        runningTaskInfo.isActive  = true;

        // Выполнение
        if (task.f) task.f();

        // Снимаем отметку
        task.running = false;
        runningTaskInfo.isActive = false;

        unsigned long endTime = micros();
        unsigned long executionTime = endTime - startTime;

        updateTaskStatistics(task.name, executionTime);

        task.lastRunTime = currentHardwareTicks;

        // Планирование следующего запуска
        if (task.interval > 0)
            task.nextRunTime = currentRealTime + task.interval;
        else
            task.nextRunTime = currentHardwareTicks + 1;

        if (task.oneShot)
            task.activ = false;
    }
}

// --- Запуск/остановка воркеров ---

void TaskDispatcher::start()
{
    if (workersRunning) return;

    if (tasksMutex == nullptr)
        tasksMutex = xSemaphoreCreateMutex();

    measurementStartTime = millis();
    lastTickRealTime     = millis();

    workersRunning = true;

    // Core 0 — обычно занят WiFi/BT, поэтому даём ему PRIORITY_LOW
    // Core 1 — основной пользовательский, PRIORITY_NORMAL
    xTaskCreatePinnedToCore(
        &TaskDispatcher::workerCore0,
        "TD_Worker_C0",
        8192,
        this,
        1,                  // низкий приоритет (idle и WiFi важнее)
        &workerCore0Handle,
        0);                 // CORE 0

    xTaskCreatePinnedToCore(
        &TaskDispatcher::workerCore1,
        "TD_Worker_C1",
        8192,
        this,
        2,                  // выше, чем у Core 0
        &workerCore1Handle,
        1);                 // CORE 1
}

void TaskDispatcher::stop()
{
    workersRunning = false;
    vTaskDelay(pdMS_TO_TICKS(50)); // дать воркерам выйти из цикла

    if (workerCore0Handle) { vTaskDelete(workerCore0Handle); workerCore0Handle = nullptr; }
    if (workerCore1Handle) { vTaskDelete(workerCore1Handle); workerCore1Handle = nullptr; }
}

// --- Совместимость: старый tick() теперь просто ничего не делает ---

void TaskDispatcher::tick()
{
    // Оставлено для совместимости.
    // Реальная работа выполняется в воркерах (workerCore0/workerCore1).
}

// --- Очистка стека форм (без изменений) ---

void TaskDispatcher::clearExFormsStack()
{
    while (!formsStack.empty())
    {
        exForm* form = formsStack.top();
        delete form;
        formsStack.pop();
    }
}

void runExFormStack()
{
    if (!formsStack.empty())
    {
        exForm *currentForm = formsStack.top();
        if (currentForm != nullptr)
        {
            int result = currentForm->showForm();
            if (result == 1)
            {
                formsStack.pop();
                delete currentForm;
                delay(250);
            }
        }
    }
    if (formsStack.empty())
    {
        _myOSstartupForm();
    }
}

// --- Точка входа, вызываемая из loop() ---

void runTasksCore()
{
#ifndef DEBUG_TASK_DISPATCHER
    // Диспетчер теперь работает на двух ядрах — здесь делать нечего.
    // (Опционально: можно оставить пустой yield.)
    vTaskDelay(1);
#else
    Serial.printf("Total tasks: %d\n", tasks.size());
    Serial.printf("Hardware ticks: %lu\n", _TD.getHardwareTicks());
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, core: %d, priority: %d, oneShot: %d, interval: %lu\n",
                      i, tasks[i].name.c_str(), tasks[i].activ,
                      (int)tasks[i].core,
                      tasks[i].priority, tasks[i].oneShot, tasks[i].interval);
    }
    vTaskDelay(1);
#endif
}

bool TaskDispatcher::terminal()
{
    _GRF.render(runTasksCore);
    return true;
}

void nullFunction() {};

// --- Статистика ---

void TaskDispatcher::updateTaskStatistics(const String &taskName, unsigned long executionTime)
{
    // Защищаем статистику мьютексом, т.к. пишут два ядра
    static SemaphoreHandle_t statsMutex = xSemaphoreCreateMutex();
    if (statsMutex && xSemaphoreTake(statsMutex, portMAX_DELAY) == pdTRUE)
    {
        if (taskStatistics.find(taskName) == taskStatistics.end())
            taskStatistics[taskName] = {0, 0, 0};

        taskStatistics[taskName].totalExecutionTime += executionTime;
        taskStatistics[taskName].callCount++;
        taskStatistics[taskName].lastExecutionTime = executionTime;

        totalExecutionTime += executionTime;
        xSemaphoreGive(statsMutex);
    }
}

int TaskDispatcher::getCPULoad()
{
    static SemaphoreHandle_t loadMutex = xSemaphoreCreateMutex();
    int cpuLoad = 0;

    if (loadMutex && xSemaphoreTake(loadMutex, portMAX_DELAY) == pdTRUE)
    {
        unsigned long currentTime = millis();
        unsigned long windowSize  = currentTime - measurementStartTime;

        if (windowSize >= 100)
        {
            unsigned long maxPossibleTime = windowSize * 1000ULL;
            if (maxPossibleTime > 0)
            {
                cpuLoad = (totalExecutionTime * 100) / maxPossibleTime;
                if (cpuLoad > 100) cpuLoad = 100;
                if (cpuLoad < 0)   cpuLoad = 0;
            }
        }
        xSemaphoreGive(loadMutex);
    }
    return cpuLoad;
}