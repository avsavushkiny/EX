#pragma once

#include <algorithm>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"
#include "esp_timer.h"

// Определение глобального вектора
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;

static unsigned long taskStartTime = 0;
static String currentTaskName = "";

// Добавляем переменные для аппаратного таймера
static esp_timer_handle_t system_timer = nullptr;
static volatile unsigned long hardwareTicks = 0;
static const unsigned long TIMER_INTERVAL_US = 1000; // 1ms в микросекундах

static const unsigned long TASK_TIME_SLICE_LIMIT_US = 900; // Лимит выполнения одной задачи в микросекундах

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

void TaskDispatcher::resetSystemClock()
{
    // Устанавливаем точку отсчета реального времени на текущий момент millis()
    lastTickRealTime = millis();                     // Аппаратные тики тоже сбрасываем к текущему значению
    unsigned long currentTicks = getHardwareTicks(); // Проходимся по всем задачам и корректируем их следующее время выполнения
    for (auto &t : tasks)
    {
        if (t.useHardwareTicks)
        {
            t.nextRunTime = currentTicks + t.interval;
        }
        else
        {
            t.nextRunTime = lastTickRealTime + t.interval;
        }
        // Снимаем статус "выполняется", если сон прервал задачу посередине
        t.isRunning = false;
    }
}

void TaskDispatcher::addTasksForSystems()
{
    for (TaskArguments &t : system0)
    {
        tasks.push_back(t);
    }
}

// Версия tick() с использованием аппаратного таймера 
// Глобальный флаг
volatile bool needsResort = true;
void TaskDispatcher::tick()
{
    unsigned long currentRealTime = millis();
    unsigned long currentHardwareTicksVal = getHardwareTicks();
    
    // Рассчитываем дельту реального времени для статистики
    unsigned long realTimeDelta = currentRealTime - lastTickRealTime;
    lastTickRealTime = currentRealTime;

    // --- ПЕРЕСОРТИРОВКА (вынесена отдельно от основного цикла) ---
    if (needsResort) {
        std::sort(tasks.begin(), tasks.end(), 
                  [](const TaskArguments &a, const TaskArguments &b) {
                      return a.priority > b.priority;
                  });
        needsResort = false;
    }

    // Бюджет времени на текущий вызов tick().
    unsigned long sliceStartTime = micros(); 
    unsigned long totalSliceUsed = 0;

    // Основной цикл диспетчеризации
    for (auto &task : tasks) 
    {
        // Пропускаем неактивные задачи
        if (!task.activ || !task.f) continue;

        // КРИТИЧЕСКОЕ ИЗМЕНЕНИЕ: 
        // Сначала проверяем бюджет, и только потом решаем, запускать ли задачу.
        // Это предотвращает "захват" процессора низкоприоритетными задачами.
        if (totalSliceUsed >= TASK_TIME_SLICE_LIMIT_US) 
        {
            break; // Отдаем управление циклу loop()
        }

        bool shouldRun = false;

        // --- ВЫБОР РЕЖИМА ПЛАНИРОВАНИЯ ---
        if (task.useHardwareTicks) 
        {
            // Режим аппаратных тиков (для курсора, опроса кнопок)
            if (currentHardwareTicksVal >= task.nextRunTime) 
            {
                shouldRun = true;
            }
        } 
        else 
        {
            // Режим реального времени (millis())
            if (task.interval > 0) 
            {
                if (currentRealTime >= task.nextRunTime) 
                {
                    shouldRun = true;
                }
            } 
            else 
            {
                // Задачи без интервала по умолчанию живут на тиках
                if (currentHardwareTicksVal >= task.nextRunTime) 
                {
                    shouldRun = true;
                }
            }
        }

        if (shouldRun) 
        {
            unsigned long startExec = micros();

#ifndef WATCHDOG
#else
            noInterrupts();
            runningTaskInfo.name = task.name;
            runningTaskInfo.startTime = millis();
            runningTaskInfo.isActive = true;
            interrupts();
#endif

            task.isRunning = true;
            
            // Выполнение пользовательского кода
            task.f(); 
            
            task.isRunning = false;

#ifndef WATCHDOG
#else
            noInterrupts();
            runningTaskInfo.isActive = false;
            interrupts();
#endif

            unsigned long endExec = micros();
            unsigned long executionTime = endExec - startExec;
            task.lastRunDuration = executionTime;

            updateTaskStatistics(task.name, executionTime);
            task.lastRunTime = currentHardwareTicksVal;

            // --- ОБНОВЛЕНИЕ СЛЕДУЮЩЕГО ЗАПУСКА ---
            if (task.useHardwareTicks) 
            {
                task.nextRunTime = currentHardwareTicksVal + task.interval; 
            } 
            else 
            {
                if (task.interval > 0) 
                {
                    task.nextRunTime = currentRealTime + task.interval; 
                } 
                else 
                {
                    task.nextRunTime = currentHardwareTicksVal + 1; 
                }
            }

            // Обработка одноразовых задач
            if (task.oneShot) 
            {
                task.activ = false;
                // ФЛАГ БЕЗОПАСНОСТИ: если задача удалила себя сама, 
                // вектор изменился, нужно пересортировать его в следующем тике.
                needsResort = true; 
            }

            // Накапливаем затраченное время для контроля лимита
            totalSliceUsed += executionTime;
        }
    }

    // Сброс окна измерения загрузки CPU
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