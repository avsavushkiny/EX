#pragma once

#include <vector>
#include <functional>
#include <string>
#include <Arduino.h>
#include <map>
#include "esp_timer.h"

// Предварительное объявление структуры задачи (важно!)
struct TaskArguments;

// Глобальные векторы задач
extern std::vector<TaskArguments> tasks;
extern std::vector<TaskArguments> userTasks;

enum TaskType
{
    SYSTEM,
    DESKTOP,
    USER
};

enum TaskPriority : int8_t
{
    PRIORITY_IDLE = 0,
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 5,
    PRIORITY_HIGH = 10,
    PRIORITY_CRITICAL = 15
};

struct TaskStatistics
{
    uint64_t totalExecutionTime = 0;
    uint32_t callCount = 0;
    uint32_t lastExecutionTime = 0;
    uint32_t deadlineMisses = 0;
};

struct TaskArguments
{
    String name;
    void (*f)(void);
    const uint8_t *bitMap;
    TaskType type;
    int index;
    bool activ;
    TaskPriority priority;
    bool oneShot;

    volatile uint32_t lastRunTime;
    volatile uint32_t nextRunTime;

    uint32_t interval;
    uint32_t timeBudgetUs;

    TaskStatistics stats;
};

class TaskDispatcher
{
public:
    TaskDispatcher();

    // --- Совместимость со старым кодом ---
    int sizeTasks();
    void addTask(const TaskArguments &task);
    bool removeTaskVector(const String &taskName);
    bool removeTask(const String &taskName);
    bool removeTaskIndex(const int index);
    bool runTask(const String &taskName);
    void clearExFormsStack();
    void addTasksForSystems();
    bool terminal();

    void tick();
    int getCPULoad();

    static void initHardwareTimer();
    static void stopHardwareTimer();
    static uint64_t getHardwareTicks();

private:
    std::vector<TaskArguments *> sortedTasks;

    struct RunningTaskInfo
    {
        String name;
        uint32_t startTime = 0;
        bool isActive = false;
    } runningTaskInfo;

    const uint32_t MAX_EXECUTION_TIME_US = 4500;

    uint64_t totalExecutionTime = 0;
    uint32_t measurementStartTime = 0;
    static const uint32_t MEASUREMENT_WINDOW = 1000;

    std::map<String, TaskStatistics> taskStatistics;

    void updateTaskStatistics(TaskArguments &task, uint32_t executionTime);
};

// Прототип функции ядра (нужен для метода terminal())
void runTasksCore();

// Глобальный экземпляр
extern TaskDispatcher _TD;