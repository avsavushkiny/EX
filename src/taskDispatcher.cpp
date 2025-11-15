#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"

// --- Глобальные переменные ---
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;

// --- Мьютекс для синхронизации доступа к вектору задач ---
SemaphoreHandle_t taskMutex = NULL;

void nullFunction(){}

// --- Инициализация мьютекса ---
void TaskDispatcher::init()
{
    if (taskMutex == NULL)
    {
        taskMutex = xSemaphoreCreateMutex();
    }
}

// --- Потокобезопасный размер ---
int TaskDispatcher::sizeTasks()
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        int size = tasks.size();
        xSemaphoreGive(taskMutex);
        return size;
    }
    return 0;
}

// --- Потокобезопасное добавление ---
void TaskDispatcher::addTask(const TaskArguments &task)
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        tasks.push_back(task);
        xSemaphoreGive(taskMutex);
    }
}

// --- Удаление из вектора по имени (полное удаление) ---
bool TaskDispatcher::removeTaskVector(const String &taskName)
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [&taskName](const TaskArguments &t)
                               { return t.name == taskName; });
        if (it != tasks.end())
        {
            tasks.erase(it);
            xSemaphoreGive(taskMutex);
            return true;
        }
        xSemaphoreGive(taskMutex);
        return false;
    }
    return false;
}

// --- Деактивация задачи по имени (не удаляя из вектора) ---
bool TaskDispatcher::removeTask(const String &taskName)
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        for (auto &t : tasks)
        {
            if (t.activ && t.name == taskName)
            {
                t.activ = false;
                xSemaphoreGive(taskMutex);
                return true;
            }
        }
        xSemaphoreGive(taskMutex);
        return false;
    }
    return false;
}

// --- Деактивация по индексу ---
bool TaskDispatcher::removeTaskIndex(const int index)
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        for (auto &t : tasks)
        {
            if (t.activ && t.index == index)
            {
                t.activ = false;
                xSemaphoreGive(taskMutex);
                return true;
            }
        }
        xSemaphoreGive(taskMutex);
        return false;
    }
    return false;
}

// --- Активация задачи ---
bool TaskDispatcher::runTask(const String &taskName)
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        for (auto &t : tasks)
        {
            if (!t.activ && t.name == taskName)
            {
                t.activ = true;
                xSemaphoreGive(taskMutex);
                return true;
            }
        }
        xSemaphoreGive(taskMutex);
        return false;
    }
    return false;
}

// --- Добавление системных задач ---
void TaskDispatcher::addTasksForSystems()
{
    if (xSemaphoreTake(taskMutex, portMAX_DELAY) == pdTRUE)
    {
        for (TaskArguments &t : system0)
        {
            tasks.push_back(t);
        }
        xSemaphoreGive(taskMutex);
    }
}

// --- Управление стеком форм ---
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

// --- Основной цикл выполнения задач (вызывается из задачи FreeRTOS) ---
void runTasksCore(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(taskMutex, 10) == pdTRUE)
        {
#ifdef DEBUG_TASK_DISPATCHER
            Serial.printf("[TaskCore] Running %d tasks...\n", tasks.size());
#endif

            for (auto &t : tasks)
            {
                if (t.activ && t.f)
                {
#ifdef DEBUG_TASK_DISPATCHER
                    Serial.printf("[Task] Executing: %s\n", t.name.c_str());
#endif
                    t.f();
                }
            }

            xSemaphoreGive(taskMutex);
        }

        // Короткая задержка, чтобы не грузить ядро
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// --- Запуск задачи FreeRTOS на ESP32 ---
bool TaskDispatcher::terminal()
{
    init(); // Инициализация мьютекса

    // Запускаем `runTasksCore` как отдельную задачу FreeRTOS
    // На ESP32 можно указать ядро, например: xTaskCreatePinnedToCore
#ifdef ESP32
    xTaskCreatePinnedToCore(
        runTasksCore,      // функция
        "TaskCore",        // имя
        8192,              // стек
        nullptr,           // параметры
        1,                 // приоритет
        nullptr,           // handle
        0                  // ядро 0
    );
#else
    xTaskCreate(
        runTasksCore,
        "TaskCore",
        4096,
        nullptr,
        1,
        nullptr
    );
#endif

    // Основной цикл продолжает работать (например, отрисовка GUI)
    _GRF.render([]()
                { /* Пустой — задачи выполняются в отдельном потоке */
                });

    return true;
}
