#pragma once

#include <algorithm>
#include <chrono>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"
#include "esp_timer.h"
#include <mutex>

extern void runTasksCore();

// Определение глобальных переменных
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;
std::mutex tasksMutex;

// Статические переменные таймера
volatile unsigned long TaskDispatcher::hardwareTicks = 0;
esp_timer_handle_t TaskDispatcher::systemTimer = nullptr;

// Дружественная функция для доступа к private членам
void IRAM_ATTR timerCallback(void *arg)
{
    // Используем прямое обращение к статической переменной
    // Она объявлена как static, поэтому доступна через класс
    TaskDispatcher::hardwareTicks++;
}

// Конструктор
TaskDispatcher::TaskDispatcher()
{
    measurementStartTime = millis();
    lastTickTime = getHardwareTicks();
    initHardwareTimer();
}

// Деструктор
TaskDispatcher::~TaskDispatcher()
{
    stopHardwareTimer();
    clearAllTasks();
}

// ============= УПРАВЛЕНИЕ ТАЙМЕРОМ =============

void TaskDispatcher::initHardwareTimer()
{
    if (systemTimer != nullptr)
    {
        return;
    }

    esp_timer_create_args_t timerArgs = {
        .callback = &timerCallback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK, // Используем TASK вместо ISR
        .name = "system_tick"};

    esp_err_t ret = esp_timer_create(&timerArgs, &systemTimer);
    if (ret == ESP_OK)
    {
        esp_timer_start_periodic(systemTimer, TIMER_INTERVAL_US);
    }
}

void TaskDispatcher::stopHardwareTimer()
{
    if (systemTimer != nullptr)
    {
        esp_timer_stop(systemTimer);
        esp_timer_delete(systemTimer);
        systemTimer = nullptr;
    }
}

unsigned long TaskDispatcher::getHardwareTicks()
{
    return hardwareTicks;
}

void TaskDispatcher::resetHardwareTicks()
{
    hardwareTicks = 0;
}

// ============= УПРАВЛЕНИЕ ЗАДАЧАМИ =============

int TaskDispatcher::sizeTasks()
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    return tasks.size();
}

void TaskDispatcher::addTask(const TaskArguments &task)
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    TaskArguments newTask = task;
    newTask.lastRunTime = 0;
    newTask.nextRunTime = getHardwareTicks() + task.interval;
    newTask.isRunning = false;
    newTask.executionTime = 0;
    tasks.push_back(newTask);
}

bool TaskDispatcher::removeTask(const String &taskName)
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    auto it = std::find_if(tasks.begin(), tasks.end(),
                           [&taskName](const TaskArguments &t)
                           {
                               return t.name == taskName;
                           });

    if (it != tasks.end())
    {
        tasks.erase(it);
        return true;
    }
    return false;
}

bool TaskDispatcher::removeTaskByIndex(const int index)
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    auto it = std::find_if(tasks.begin(), tasks.end(),
                           [index](const TaskArguments &t)
                           {
                               return t.index == index;
                           });

    if (it != tasks.end())
    {
        tasks.erase(it);
        return true;
    }
    return false;
}

bool TaskDispatcher::activateTask(const String &taskName)
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    for (auto &task : tasks)
    {
        if (task.name == taskName && !task.activ)
        {
            task.activ = true;
            task.nextRunTime = getHardwareTicks() + task.interval;
            return true;
        }
    }
    return false;
}

bool TaskDispatcher::deactivateTask(const String &taskName)
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    for (auto &task : tasks)
    {
        if (task.name == taskName && task.activ)
        {
            task.activ = false;
            task.isRunning = false;
            return true;
        }
    }
    return false;
}

void TaskDispatcher::clearAllTasks()
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    tasks.clear();
    userTasks.clear();
}

void TaskDispatcher::addTasksForSystems()
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    for (TaskArguments &t : system0)
    {
        TaskArguments newTask = t;
        newTask.lastRunTime = 0;
        newTask.nextRunTime = getHardwareTicks() + t.interval;
        newTask.isRunning = false;
        newTask.executionTime = 0;
        tasks.push_back(newTask);
    }
}

// ============= ОСНОВНОЙ ЦИКЛ ДИСПЕТЧЕРА =============

void TaskDispatcher::tick()
{
    unsigned long currentTick = getHardwareTicks();
    unsigned long tickDelta = currentTick - lastTickTime;
    lastTickTime = currentTick;

    // Сбор активных задач
    std::vector<TaskArguments *> activeTasks;
    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        for (auto &task : tasks)
        {
            if (task.activ && task.f != nullptr && !task.isRunning)
            {
                activeTasks.push_back(&task);
            }
        }
    }

    // Сортировка по приоритету
    std::sort(activeTasks.begin(), activeTasks.end(),
              [](const TaskArguments *a, const TaskArguments *b)
              {
                  return a->priority > b->priority;
              });

    // Выполнение задач
    for (auto taskPtr : activeTasks)
    {
        auto &task = *taskPtr;

        // Проверка времени выполнения
        if (!shouldRunTask(task, currentTick))
        {
            continue;
        }

        // Вытесняющее выполнение
        executeTask(task);

        // Обновление расписания
        updateTaskSchedule(task, currentTick);

        // Одноразовые задачи
        if (task.oneShot)
        {
            std::lock_guard<std::mutex> lock(tasksMutex);
            task.activ = false;
        }
    }

    // Сброс статистики
    unsigned long currentTime = millis();
    if (currentTime - measurementStartTime >= MEASUREMENT_WINDOW)
    {
        measurementStartTime = currentTime;
        totalExecutionTime = 0;
    }
}

bool TaskDispatcher::shouldRunTask(const TaskArguments &task, unsigned long currentTick)
{
    // Задачи с интервалом запускаются по расписанию
    if (task.interval > 0)
    {
        return currentTick >= task.nextRunTime;
    }

    // Задачи без интервала запускаются каждый тик
    return true;
}

void TaskDispatcher::executeTask(TaskArguments &task)
{
    unsigned long startTime = micros();
    unsigned long budgetStart = getHardwareTicks();

    // Устанавливаем флаг выполнения
    task.isRunning = true;

    // Выполняем задачу с контролем времени
    while (true)
    {
        // Выполняем задачу
        if (task.f)
        {
            task.f();
        }

        // Проверяем, не превышен ли бюджет времени
        unsigned long currentTime = micros();
        unsigned long executionTime = currentTime - startTime;

        if (executionTime >= taskBudget)
        {
            // Вытеснение задачи
            task.executionTime = executionTime;
            break;
        }

        // Если задача завершилась быстро - выходим
        if (!task.isRunning)
        {
            task.executionTime = executionTime;
            break;
        }

        // Проверяем, не истек ли квант времени
        unsigned long currentTick = getHardwareTicks();
        if (currentTick - budgetStart >= schedulerQuantum / 1000) // Конвертация в тики
        {
            // Даем возможность другим задачам выполниться
            task.isRunning = false;
            delay(1); // Минимальная задержка для переключения контекста
            task.isRunning = true;
            budgetStart = currentTick;
        }
    }

    // Снимаем флаг выполнения
    task.isRunning = false;

    // Обновляем статистику
    unsigned long endTime = micros();
    unsigned long executionTime = endTime - startTime;
    updateTaskStatistics(task.name, executionTime);
    totalExecutionTime += executionTime;
}

void TaskDispatcher::updateTaskSchedule(TaskArguments &task, unsigned long currentTick)
{
    if (task.interval > 0)
    {
        task.lastRunTime = currentTick;
        task.nextRunTime = currentTick + task.interval;
    }
    else
    {
        task.lastRunTime = currentTick;
        task.nextRunTime = currentTick + 1;
    }
}

// ============= УПРАВЛЕНИЕ ВЫТЕСНЕНИЕМ =============

void TaskDispatcher::setTaskBudget(unsigned long budgetMicroseconds)
{
    taskBudget = budgetMicroseconds;
}

unsigned long TaskDispatcher::getTaskBudget() const
{
    return taskBudget;
}

void TaskDispatcher::setSchedulerQuantum(unsigned long quantumMicroseconds)
{
    schedulerQuantum = quantumMicroseconds;
}

unsigned long TaskDispatcher::getSchedulerQuantum() const
{
    return schedulerQuantum;
}

// ============= СТАТИСТИКА =============

void TaskDispatcher::updateTaskStatistics(const String &taskName, unsigned long executionTime)
{
    std::lock_guard<std::mutex> lock(statsMutex);

    auto it = taskStatistics.find(taskName);
    if (it == taskStatistics.end())
    {
        TaskStats stats;
        stats.totalExecutionTime = executionTime;
        stats.callCount = 1;
        stats.lastExecutionTime = executionTime;
        stats.maxExecutionTime = executionTime;
        stats.minExecutionTime = executionTime;
        taskStatistics[taskName] = stats;
    }
    else
    {
        it->second.totalExecutionTime += executionTime;
        it->second.callCount++;
        it->second.lastExecutionTime = executionTime;
        it->second.maxExecutionTime = std::max(it->second.maxExecutionTime, executionTime);
        it->second.minExecutionTime = std::min(it->second.minExecutionTime, executionTime);
    }
}

int TaskDispatcher::getCPULoad()
{
    unsigned long currentTime = millis();
    unsigned long windowSize = currentTime - measurementStartTime;

    if (windowSize < 100)
    {
        static int lastLoad = 0;
        return lastLoad;
    }

    unsigned long maxPossibleTime = windowSize * 1000;
    int cpuLoad = 0;

    if (maxPossibleTime > 0)
    {
        cpuLoad = (totalExecutionTime * 100) / maxPossibleTime;
        cpuLoad = std::min(100, std::max(0, cpuLoad));
    }

    return cpuLoad;
}

void TaskDispatcher::resetStatistics()
{
    std::lock_guard<std::mutex> lock(statsMutex);
    taskStatistics.clear();
    totalExecutionTime = 0;
    measurementStartTime = millis();
}

// ============= ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =============

bool TaskDispatcher::terminal()
{
#ifdef DEBUG_TASK_DISPATCHER
    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        Serial.printf("Total tasks: %d\n", tasks.size());
        Serial.printf("Hardware ticks: %lu\n", getHardwareTicks());

        for (size_t i = 0; i < tasks.size(); ++i)
        {
            auto &task = tasks[i];
            Serial.printf("Task %d: %s, active: %d, priority: %d, interval: %lu, running: %d\n",
                          i, task.name.c_str(), task.activ, task.priority, task.interval, task.isRunning);
        }
    }
#endif

    // Вызов runTasksCore через внешнюю функцию
    extern void runTasksCore();
    _GRF.render(runTasksCore);
    return true;
}

void nullFunction() {}

// ============= ГЛОБАЛЬНЫЕ ФУНКЦИИ =============

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
                delay(1);
            }
        }
    }
    if (formsStack.empty())
    {
        _myOSstartupForm();
    }
}

void runTasksCore()
{
    _TD.tick();
}