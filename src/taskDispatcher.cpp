#pragma once
#define DEBUG_TASK_DISPATCHER


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

// Глобальный флаг
volatile bool needsResort = true;

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

void TaskDispatcher::handleTimeSync(unsigned long currentRealTime, unsigned long currentHardwareTicks)
{
    // Вычисляем разницу в аппаратных тиках
    long tickDelta = (long)currentHardwareTicks - (long)lastHardwareTickSeen;

    // Защита от переполнения или первого запуска
    if (lastHardwareTickSeen == 0 || tickDelta < 0) 
    {
        lastHardwareTickSeen = currentHardwareTicks;
        
        // При самом первом запуске принудительно выравниваем расписание всех задач
        for (auto &task : tasks) 
        {
            if (task.useHardwareTicks) 
            {
                task.nextRunTime = currentHardwareTicks + task.interval;
            } 
            else 
            {
                task.nextRunTime = currentRealTime + task.interval;
            }
        }
        return;
    }

    // Порог "скачка". 
    // Если за один цикл loop/tick прошло больше 500 мс (500 тиков по 1000 мкс),
    // значит процессор находился в глубоком сне или был заблокирован критической секцией.
    const unsigned long SLEEP_THRESHOLD_TICKS = 500; 

    if ((unsigned long)tickDelta > SLEEP_THRESHOLD_TICKS) 
    {
        // --- ОБНАРУЖЕНО ПРОБУЖДЕНИЕ ---
        
        // Сбрасываем статистику CPU, так как она неактуальна после долгого простоя
        measurementStartTime = currentRealTime;
        totalExecutionTime = 0;

        // Корректируем планировщик: переносим все задачи в "настоящее"
        for (auto &task : tasks) 
        {
            if (task.activ) 
            {
                if (task.useHardwareTicks) 
                {
                    // Устанавливаем следующее выполнение на ближайшее будущее
                    task.nextRunTime = currentHardwareTicks + 1; 
                } 
                else 
                {
                    task.nextRunTime = currentRealTime + 1;
                }
                
                // Сбрасываем длительность выполнения, чтобы getCPULoad() не показал 100%
                task.lastRunDuration = 0; 
            }
        }
        
        // Флаг пересортировки нужен, если какие-то задачи могли изменить свой статус во время сна
        needsResort = true; 
    }

    // Обновляем счетчик для следующей проверки
    lastHardwareTickSeen = currentHardwareTicks;
}

void TaskDispatcher::addTasksForSystems()
{
    for (TaskArguments &t : system0)
    {
        tasks.push_back(t);
    }
}

// Версия tick() с использованием аппаратного таймера 
void TaskDispatcher::tick()
{
    unsigned long currentRealTime = millis();
    unsigned long currentHardwareTicksVal = getHardwareTicks();

    // 1. АВТОМАТИЧЕСКАЯ СИНХРОНИЗАЦИЯ ВРЕМЕНИ
    // Вызываем функцию до начала планирования. Она сама обнаружит сон.
    handleTimeSync(currentRealTime, currentHardwareTicksVal);

    // Рассчитываем реальное время с последнего тика (для статистики окна измерения)
    unsigned long realTimeDelta = currentRealTime - lastTickRealTime;
    lastTickRealTime = currentRealTime;

    // 2. ПЕРЕСОРТИРОВКА ПО ПРИОРИТЕТАМ
    if (needsResort) 
    {
        std::sort(tasks.begin(), tasks.end(), 
                  [](const TaskArguments &a, const TaskArguments &b) {
                      return a.priority > b.priority;
                  });
        needsResort = false;
    }

    // 3. ЦИКЛ ВЫПОЛНЕНИЯ
    // Бюджет времени на текущий вызов tick().
    unsigned long sliceStartTime = micros(); 
    unsigned long totalSliceUsed = 0;
    bool backgroundBudgetExhausted = false;

    for (auto &task : tasks) 
    {
        if (!task.activ || !task.f) continue;

        // Проверка лимита среза только для фоновых задач
        if (backgroundBudgetExhausted && task.priority < PRIORITY_HIGH) 
        {
            continue;
        }

        bool shouldRun = false;
        if (task.useHardwareTicks) 
        {
            shouldRun = (currentHardwareTicksVal >= task.nextRunTime);
        } 
        else 
        {
            if (task.interval > 0) 
            {
                shouldRun = (currentRealTime >= task.nextRunTime);
            } 
            else 
            {
                shouldRun = (currentHardwareTicksVal >= task.nextRunTime);
            }
        }

        if (shouldRun) 
        {
            unsigned long allowedSlice = (task.priority < PRIORITY_HIGH) ? 1500 : 0;
            
            if (allowedSlice != 0 && totalSliceUsed >= 1500) 
            {
                backgroundBudgetExhausted = true;
                continue;
            }

            noInterrupts();
            runningTaskInfo.name = task.name;
            runningTaskInfo.startTime = millis();
            runningTaskInfo.isActive = true;
            interrupts();

            task.isRunning = true;
            unsigned long execStart = micros();
            
            task.f(); Serial.println("tick");
            
            task.isRunning = false;
            unsigned long execEnd = micros();
            unsigned long executionTime = execEnd - execStart;

            noInterrupts();
            runningTaskInfo.isActive = false;
            interrupts();

            updateTaskStatistics(task.name, executionTime);
            task.lastRunDuration = executionTime;
            task.lastRunTime = currentHardwareTicksVal;

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

            if (task.oneShot) 
            {
                task.activ = false;
                needsResort = true;
            }

            if (allowedSlice != 0) 
            {
                totalSliceUsed += executionTime;
            }
        }
    }

    // Статистика загрузки CPU
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