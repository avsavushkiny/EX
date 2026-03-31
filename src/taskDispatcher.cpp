#pragma once

#include <algorithm>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"
#include "esp_timer.h"
#include <queue>

// Глобальный экземпляр многозадачного диспетчера
MultiTaskingDispatcher MTD;

// Определение глобального вектора
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;

static unsigned long taskStartTime = 0;
static String currentTaskName = "";

// Добавляем переменные для аппаратного таймера
static esp_timer_handle_t system_timer = nullptr;
static volatile unsigned long hardwareTicks = 0;
static const unsigned long TIMER_INTERVAL_US = 1000; // 1ms в микросекундах

// Callback для аппаратного таймера
void system_timer_callback(void *arg)
{
    hardwareTicks++;
}

// Инициализация аппаратного таймера
void TaskDispatcher::initHardwareTimer()
{
    if (system_timer != nullptr)
    {
        return; // Таймер уже инициализирован
    }

    esp_timer_create_args_t timer_args = {
        .callback = &system_timer_callback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "system_tick"};

    esp_err_t ret = esp_timer_create(&timer_args, &system_timer);
    if (ret != ESP_OK)
    {
        // Обработка ошибки создания таймера
        return;
    }

    ret = esp_timer_start_periodic(system_timer, TIMER_INTERVAL_US);
    if (ret != ESP_OK)
    {
        // Обработка ошибки запуска таймера
        esp_timer_delete(system_timer);
        system_timer = nullptr;
    }
}

// Остановка аппаратного таймера
void TaskDispatcher::stopHardwareTimer()
{
    if (system_timer != nullptr)
    {
        esp_timer_stop(system_timer);
        // esp_timer_delete(system_timer); // Не удаляем таймер (231125)
        // system_timer = nullptr;
    }
}

// Получение тиков из аппаратного таймера
unsigned long TaskDispatcher::getHardwareTicks()
{
    return hardwareTicks;
}

int TaskDispatcher::sizeTasks()
{
    return tasks.size();
}

void TaskDispatcher::addTask(const TaskArguments &task)
{
    tasks.push_back(task);
}

bool TaskDispatcher::removeTaskVector(const String &taskName)
{
    auto it = std::find_if(tasks.begin(), tasks.end(),
                           [&taskName](const TaskArguments &t)
                           { return t.name == taskName; });
    if (it != tasks.end())
    {
        tasks.erase(it);
        return true;
    }
    return false;
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
    {
        tasks.push_back(t);
    }
}

// Модифицированная версия tick() с использованием аппаратного таймера
void TaskDispatcher::tick()
{
    unsigned long currentRealTime = millis();

    // Используем аппаратные тики вместо systemTicks++
    unsigned long currentHardwareTicks = getHardwareTicks();

    // Рассчитываем реальное время с последнего тика (для интервалов)
    unsigned long realTimeDelta = currentRealTime - lastTickRealTime;
    lastTickRealTime = currentRealTime;

    // Сортируем и выполняем задачи...
    std::vector<TaskArguments *> sortedTasks;
    for (auto &task : tasks)
    {
        if (task.activ && task.f)
        {
            sortedTasks.push_back(&task);
        }
    }

    std::sort(sortedTasks.begin(), sortedTasks.end(),
              [](const TaskArguments *a, const TaskArguments *b)
              {
                  return a->priority > b->priority;
              });

    // Выполняем задачи
    for (auto taskPtr : sortedTasks)
    {
        auto &task = *taskPtr;

        // Используем аппаратные тики для планирования
        bool shouldRun = false;
        if (task.interval > 0)
        {
            // Для периодических задач используем реальное время
            shouldRun = (currentRealTime >= task.nextRunTime);
        }
        else
        {
            // Для задач без интервала используем аппаратные тики
            shouldRun = (currentHardwareTicks >= task.nextRunTime);
        }

        if (shouldRun)
        {
            unsigned long startTime = micros();
            currentTaskName = task.name;

// Код для диспетчера задач, реализация tick
// Фиксируем начало выполнения задачи
// #ifndef WATCHDOG
// #else
            noInterrupts();
            runningTaskInfo.name = task.name;
            runningTaskInfo.startTime = millis();
            runningTaskInfo.isActive = true;
            interrupts();
// #endif

            // Выполняем задачу
            if (task.f)
            {
                task.f();
            }

// #ifndef WATCHDOG
// #else
            // Задача завершилась — сбрасываем флаг
            noInterrupts();
            runningTaskInfo.isActive = false;
            interrupts();
// #endif
            //--

            unsigned long endTime = micros();
            unsigned long executionTime = endTime - startTime;

            // Обновляем статистику
            updateTaskStatistics(task.name, executionTime);

            task.lastRunTime = currentHardwareTicks;

            // Планируем следующее выполнение
            if (task.interval > 0)
            {
                task.nextRunTime = currentRealTime + task.interval;
            }
            else
            {
                task.nextRunTime = currentHardwareTicks + 1;
            }

            if (task.oneShot)
            {
                task.activ = false;
            }
        }
    }

    // Сброс статистики каждую секунду
    if (currentRealTime - measurementStartTime >= MEASUREMENT_WINDOW)
    {
        measurementStartTime = currentRealTime;
        totalExecutionTime = 0;
    }
}

void TaskDispatcher::clearExFormsStack()
{
    while (!formsStack.empty())
    {
        exForm* form = formsStack.top();
        delete form;           // Освобождаем память
        formsStack.pop();      // Удаляем указатель из стека
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

/* Core tasks with debug */
void runTasksCore()
{
#ifndef DEBUG_TASK_DISPATCHER
    _TD.tick(); // Используем новый тиковый диспетчер с аппаратным таймером
    // runExFormStack();
#else
    Serial.printf("Total tasks: %d\n", tasks.size());
    Serial.printf("Hardware ticks: %lu\n", _TD.getHardwareTicks());
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, priority: %d, oneShot: %d, interval: %lu\n",
                      i, tasks[i].name.c_str(), tasks[i].activ,
                      tasks[i].priority, tasks[i].oneShot, tasks[i].interval);
    }

    _TD.tick(); // Используем новый тиковый диспетчер с аппаратным таймером
#endif
}

/* Terminal with debug */
bool TaskDispatcher::terminal()
{
#ifndef DEBUG_TASK_DISPATCHER
    _GRF.render(runTasksCore);
    return true;
#else
    Serial.printf("Total tasks: %d\n", tasks.size());
    Serial.printf("Hardware ticks: %lu\n", getHardwareTicks());
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, func: %p\n",
                      i, tasks[i].name.c_str(), tasks[i].activ, tasks[i].f);
    }

    _GRF.render(runTasksCore);
    return true;
#endif
}

void nullFunction() {};

// Статистика
void TaskDispatcher::updateTaskStatistics(const String &taskName, unsigned long executionTime)
{
    if (taskStatistics.find(taskName) == taskStatistics.end())
    {
        taskStatistics[taskName] = {0, 0, 0};
    }

    taskStatistics[taskName].totalExecutionTime += executionTime;
    taskStatistics[taskName].callCount++;
    taskStatistics[taskName].lastExecutionTime = executionTime;

    totalExecutionTime += executionTime;
}

// Статистика
int TaskDispatcher::getCPULoad()
{
    unsigned long currentTime = millis();
    unsigned long windowSize = currentTime - measurementStartTime;

    if (windowSize < 100)
    {
        // Слишком маленькое окно измерения
        static int lastLoad = 0;
        return lastLoad;
    }

    // Рассчитываем загрузку CPU
    // totalExecutionTime в микросекундах, windowSize в миллисекундах
    unsigned long maxPossibleTime = windowSize * 1000; // Максимальное время в мкс
    int cpuLoad = 0;

    if (maxPossibleTime > 0)
    {
        cpuLoad = (totalExecutionTime * 100) / maxPossibleTime;
        cpuLoad = (cpuLoad > 100) ? 100 : cpuLoad;
        cpuLoad = (cpuLoad < 0) ? 0 : cpuLoad;
    }

    return cpuLoad;
}

/* MTD */
// Реализация методов TaskScheduler
void TaskScheduler::setStrategy(ScheduleStrategy strategy)
{
    currentStrategy = strategy;
}

void TaskScheduler::setTimeSlice(unsigned long microseconds)
{
    timeSlice = microseconds;
}

void TaskScheduler::yield()
{
    // Просто флаг для планировщика
    // Фактическое переключение происходит в tick()
}

// Реализация MultiTaskingDispatcher
void MultiTaskingDispatcher::updateActiveTasksCache()
{
    activeTasks.clear();
    for (auto &task : tasks)
    {
        if (task.activ && task.f)
        {
            activeTasks.push_back(&task);
        }
    }

    // Сортируем по приоритету для PRIORITY_BASED стратегии
    std::sort(activeTasks.begin(), activeTasks.end(),
              [](const TaskArguments *a, const TaskArguments *b)
              {
                  return a->priority > b->priority;
              });
}

void MultiTaskingDispatcher::executeTask(TaskArguments &task)
{
    unsigned long startTime = micros();
    currentTaskName = task.name;

    // Сохраняем контекст
    auto &ctx = taskContexts[task.name];
    ctx.lastYieldTime = startTime;
    ctx.needsYield = false;

    // Устанавливаем флаг выполнения
    noInterrupts();
    runningTaskInfo.name = task.name;
    runningTaskInfo.startTime = millis();
    runningTaskInfo.isActive = true;
    interrupts();

    // Выполняем задачу
    if (task.f)
    {
        task.f();
    }

    // Проверяем, не была ли задача прервана по таймауту
    if (!ctx.needsYield)
    {
        unsigned long endTime = micros();
        unsigned long executionTime = endTime - startTime;

        // Обновляем статистику
        updateTaskStatistics(task.name, executionTime);
        ctx.totalExecutionTime += executionTime;
        ctx.yieldCount++;

        task.lastRunTime = getHardwareTicks();

        // Планируем следующее выполнение
        if (task.interval > 0)
        {
            task.nextRunTime = millis() + task.interval;
        }
        else
        {
            task.nextRunTime = getHardwareTicks() + 1;
        }

        if (task.oneShot)
        {
            task.activ = false;
        }
    }

    // Сбрасываем флаг выполнения
    noInterrupts();
    runningTaskInfo.isActive = false;
    interrupts();
}

bool MultiTaskingDispatcher::checkTaskTimeout(const String &taskName, unsigned long startTime)
{
    auto &ctx = taskContexts[taskName];
    unsigned long currentTime = micros();

    // Проверяем превышение кванта времени для TIME_SLICING
    if (scheduler.currentStrategy == TaskScheduler::TIME_SLICING)
    {
        if (currentTime - startTime > scheduler.timeSlice)
        {
            ctx.needsYield = true;
            return true;
        }
    }

    // Проверяем превышение максимального времени выполнения
    if (ctx.maxExecutionTime > 0 && currentTime - startTime > ctx.maxExecutionTime)
    {
        Serial.printf("Warning: Task %s exceeded max execution time\n", taskName.c_str());
        ctx.needsYield = true;
        return true;
    }

    return false;
}

void MultiTaskingDispatcher::tick()
{
    unsigned long currentHardwareTicks = getHardwareTicks();
    unsigned long currentRealTime = millis();

    // Обновляем кэш активных задач
    updateActiveTasksCache();

    if (activeTasks.empty())
    {
        return;
    }

    switch (scheduler.currentStrategy)
    {
    case TaskScheduler::ROUND_ROBIN:
    {
        // Циклическое выполнение по одной задаче за тик
        if (scheduler.currentTaskIndex >= activeTasks.size())
        {
            scheduler.currentTaskIndex = 0;
        }

        auto taskPtr = activeTasks[scheduler.currentTaskIndex];
        bool shouldRun = false;

        if (taskPtr->interval > 0)
        {
            shouldRun = (currentRealTime >= taskPtr->nextRunTime);
        }
        else
        {
            shouldRun = (currentHardwareTicks >= taskPtr->nextRunTime);
        }

        if (shouldRun)
        {
            executeTask(*taskPtr);
        }

        scheduler.currentTaskIndex = (scheduler.currentTaskIndex + 1) % activeTasks.size();
        break;
    }

    case TaskScheduler::PRIORITY_BASED:
    {
        // Выполняем задачи по приоритетам (уже отсортированы)
        for (auto taskPtr : activeTasks)
        {
            bool shouldRun = false;

            if (taskPtr->interval > 0)
            {
                shouldRun = (currentRealTime >= taskPtr->nextRunTime);
            }
            else
            {
                shouldRun = (currentHardwareTicks >= taskPtr->nextRunTime);
            }

            if (shouldRun)
            {
                executeTask(*taskPtr);
                break; // Выполняем только одну задачу за тик
            }
        }
        break;
    }

    case TaskScheduler::TIME_SLICING:
    {
        // Квантование времени - выполняем задачи пока не истечет квант
        unsigned long sliceStartTime = micros();

        while ((micros() - sliceStartTime) < scheduler.timeSlice)
        {
            bool taskExecuted = false;

            for (auto taskPtr : activeTasks)
            {
                bool shouldRun = false;

                if (taskPtr->interval > 0)
                {
                    shouldRun = (currentRealTime >= taskPtr->nextRunTime);
                }
                else
                {
                    shouldRun = (currentHardwareTicks >= taskPtr->nextRunTime);
                }

                if (shouldRun)
                {
                    // Проверяем, не превышен ли квант перед выполнением
                    if ((micros() - sliceStartTime) >= scheduler.timeSlice)
                    {
                        return;
                    }

                    executeTask(*taskPtr);
                    taskExecuted = true;
                    break; // Выполнили одну задачу
                }
            }

            // Если нет готовых задач, выходим
            if (!taskExecuted)
            {
                break;
            }
        }
        break;
    }

    case TaskScheduler::COOPERATIVE:
    default:
    {
        // Оригинальное поведение - выполняем все готовые задачи
        for (auto taskPtr : activeTasks)
        {
            bool shouldRun = false;

            if (taskPtr->interval > 0)
            {
                shouldRun = (currentRealTime >= taskPtr->nextRunTime);
            }
            else
            {
                shouldRun = (currentHardwareTicks >= taskPtr->nextRunTime);
            }

            if (shouldRun)
            {
                executeTask(*taskPtr);
            }
        }
        break;
    }
    }

    // Сброс статистики каждую секунду
    if (currentRealTime - measurementStartTime >= MEASUREMENT_WINDOW)
    {
        measurementStartTime = currentRealTime;
        totalExecutionTime = 0;
    }
}

void MultiTaskingDispatcher::runMultiTasking()
{
    while (true)
    {
        tick();

        // Небольшая задержка для предотвращения 100% загрузки CPU
        // Можно настроить в зависимости от требований
        delayMicroseconds(100);
    }
}

void MultiTaskingDispatcher::yield()
{
    // Сохраняем контекст текущей задачи
    if (!currentTaskName.isEmpty())
    {
        taskContexts[currentTaskName].needsYield = true;
    }
}

bool MultiTaskingDispatcher::suspendTask(const String &taskName)
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

bool MultiTaskingDispatcher::resumeTask(const String &taskName)
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

bool MultiTaskingDispatcher::setTaskPriority(const String &taskName, TaskPriority priority)
{
    for (auto &t : tasks)
    {
        if (t.name == taskName)
        {
            t.priority = priority;
            return true;
        }
    }
    return false;
}

void MultiTaskingDispatcher::setSchedulerStrategy(TaskScheduler::ScheduleStrategy strategy)
{
    scheduler.setStrategy(strategy);
}

void MultiTaskingDispatcher::setTimeSlice(unsigned long microseconds)
{
    scheduler.setTimeSlice(microseconds);
}

// Модифицируем существующую функцию runTasksCore для использования многозадачности
void runTasksCore()
{
#ifndef DEBUG_TASK_DISPATCHER
    MTD.tick(); // Используем многозадачный диспетчер
#else
    Serial.printf("Total tasks: %d\n", tasks.size());
    Serial.printf("Active tasks: %d\n", MTD.activeTasks.size());
    Serial.printf("Hardware ticks: %lu\n", _TD.getHardwareTicks());
    Serial.printf("Scheduler strategy: %d\n", MTD.scheduler.currentStrategy);

    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, priority: %d, oneShot: %d, interval: %lu\n",
                      i, tasks[i].name.c_str(), tasks[i].activ,
                      tasks[i].priority, tasks[i].oneShot, tasks[i].interval);
    }

    MTD.tick();
#endif
}

// Добавляем вспомогательные функции для создания задач
namespace TaskFactory
{
    TaskArguments createTask(const String &name, void (*f)(void),
                             TaskPriority priority = PRIORITY_NORMAL,
                             unsigned long interval = 0,
                             bool oneShot = false)
    {
        TaskArguments task;
        task.name = name;
        task.f = f;
        task.priority = priority;
        task.interval = interval;
        task.oneShot = oneShot;
        task.activ = true;
        task.type = USER;
        task.nextRunTime = 0;
        task.lastRunTime = 0;
        return task;
    }

    TaskArguments createOneShotTask(const String &name, void (*f)(void),
                                    TaskPriority priority = PRIORITY_NORMAL)
    {
        return createTask(name, f, priority, 0, true);
    }

    TaskArguments createPeriodicTask(const String &name, void (*f)(void),
                                     unsigned long intervalMs,
                                     TaskPriority priority = PRIORITY_NORMAL)
    {
        return createTask(name, f, priority, intervalMs, false);
    }
}

// Пример использования: MTD
// void setup()
// {
//     MTD.initHardwareTimer();

//     // Создаем задачи
//     auto task1 = TaskFactory::createPeriodicTask("LED", blinkLED, 500, PRIORITY_LOW);
//     auto task2 = TaskFactory::createOneShotTask("INIT", initializeSensor, PRIORITY_HIGH);

//     MTD.addTask(task1);
//     MTD.addTask(task2);

//     // Настраиваем планировщик
//     MTD.setSchedulerStrategy(TaskScheduler::TIME_SLICING);
//     MTD.setTimeSlice(10000); // 10 мс квант
// }

// void loop()
// {
//     MTD.tick();
//     // или MTD.runMultiTasking(); // бесконечный цикл
// }