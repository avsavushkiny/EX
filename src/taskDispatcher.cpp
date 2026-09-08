#include "taskDispatcher.h"
#include "systems.h"
#include "task.h"
#include "ex.h"
#include "esp_timer.h"

std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;

TaskDispatcher _TD;

// ==================== АППАРАТНЫЙ ТАЙМЕР ESP-IDF ====================
static esp_timer_handle_t system_timer = nullptr;
static volatile uint64_t hardwareTicks = 0;
static const uint64_t TIMER_INTERVAL_US = 1000;

static void IRAM_ATTR system_timer_callback(void *arg)
{
    hardwareTicks++;
}

TaskDispatcher::TaskDispatcher()
{
    measurementStartTime = millis();
}

void TaskDispatcher::initHardwareTimer()
{
    if (system_timer != nullptr)
        return;

    esp_timer_create_args_t timer_args = {
        .callback = &system_timer_callback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "system_tick"};

    if (esp_timer_create(&timer_args, &system_timer) == ESP_OK)
    {
        esp_timer_start_periodic(system_timer, TIMER_INTERVAL_US);
    }
}

void TaskDispatcher::stopHardwareTimer()
{
    if (system_timer != nullptr)
    {
        esp_timer_stop(system_timer);
        esp_timer_delete(system_timer);
        system_timer = nullptr;
    }
}

uint64_t TaskDispatcher::getHardwareTicks()
{
    return hardwareTicks;
}

int TaskDispatcher::sizeTasks() { return tasks.size(); }

void TaskDispatcher::addTask(const TaskArguments &task) { tasks.push_back(task); }

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
        if (t.name == taskName)
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
        if (t.index == index)
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
            t.lastRunTime = hardwareTicks;
            t.nextRunTime = hardwareTicks + 1;
            return true;
        }
    }
    return false;
}

// Исправленная функция добавления системных задач
void TaskDispatcher::addTasksForSystems()
{
    int count = sizeof(system0) / sizeof(system0[0]);
    for (int i = 0; i < count; ++i)
    {
        tasks.push_back(system0[i]);
    }
}

void TaskDispatcher::clearExFormsStack()
{
    while (!formsStack.empty())
    {
        exForm *form = formsStack.top();
        delete form;
        formsStack.pop();
    }
}

void TaskDispatcher::updateTaskStatistics(TaskArguments &task, uint32_t executionTime)
{
    task.stats.totalExecutionTime += executionTime;
    task.stats.callCount++;
    task.stats.lastExecutionTime = executionTime;

    if (taskStatistics.find(task.name) == taskStatistics.end())
    {
        taskStatistics[task.name] = {0, 0, 0};
    }
    taskStatistics[task.name].totalExecutionTime += executionTime;
    taskStatistics[task.name].callCount++;
    taskStatistics[task.name].lastExecutionTime = executionTime;

    totalExecutionTime += executionTime;
}

int TaskDispatcher::getCPULoad()
{
    uint32_t currentTime = millis();
    uint32_t windowSize = currentTime - measurementStartTime;

    if (windowSize < 100)
        return 0;

    uint64_t maxPossibleTime = (uint64_t)windowSize * 1000;
    if (maxPossibleTime == 0)
        return 0;

    int cpuLoad = (int)((totalExecutionTime * 100) / maxPossibleTime);
    return (cpuLoad > 100) ? 100 : (cpuLoad < 0) ? 0
                                                 : cpuLoad;
}

// Реализация терминала
bool TaskDispatcher::terminal()
{
#ifndef DEBUG_TASK_DISPATCHER
    _GRF.render(runTasksCore);
    return true;
#else
    Serial.printf("Total tasks: %d\n", tasks.size());
    Serial.printf("Hardware ticks: %llu\n", getHardwareTicks());
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, prio: %d, budget: %lu us, misses: %lu\n",
                      i, tasks[i].name.c_str(), tasks[i].activ,
                      tasks[i].priority, tasks[i].timeBudgetUs, tasks[i].stats.deadlineMisses);
    }
    _GRF.render(runTasksCore);
    return true;
#endif
}

void TaskDispatcher::tick()
{
    uint64_t currentHardwareTicks = hardwareTicks;
    uint32_t currentRealTime = millis();

    sortedTasks.clear();
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

    for (auto taskPtr : sortedTasks)
    {
        TaskArguments &task = *taskPtr;

        bool shouldRun = false;
        if (task.interval > 0)
        {
            shouldRun = (currentRealTime >= task.nextRunTime);
        }
        else
        {
            shouldRun = (currentHardwareTicks >= task.nextRunTime);
        }

        if (shouldRun)
        {
            uint32_t startTimeUs = micros();

            portENTER_CRITICAL(nullptr);
            runningTaskInfo.name = task.name;
            runningTaskInfo.startTime = startTimeUs;
            runningTaskInfo.isActive = true;
            portEXIT_CRITICAL(nullptr);

            task.f();

            uint32_t endTimeUs = micros();
            uint32_t executionTime = endTimeUs - startTimeUs;

            portENTER_CRITICAL(nullptr);
            runningTaskInfo.isActive = false;
            portEXIT_CRITICAL(nullptr);

            updateTaskStatistics(task, executionTime);
            task.lastRunTime = currentHardwareTicks;

            bool budgetExceeded = false;
            if (task.timeBudgetUs > 0 && executionTime > task.timeBudgetUs)
            {
                task.stats.deadlineMisses++;
                budgetExceeded = true;
            }

            if (budgetExceeded)
            {
                task.activ = false;
            }

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

            if (executionTime > MAX_EXECUTION_TIME_US)
            {
                totalExecutionTime = 0;
                measurementStartTime = currentRealTime;
            }
        }
    }

    if (currentRealTime - measurementStartTime >= MEASUREMENT_WINDOW)
    {
        measurementStartTime = currentRealTime;
        totalExecutionTime = 0;
    }
}

// Определение функции core (теперь она вне класса)
void runTasksCore()
{
    _TD.tick();
}