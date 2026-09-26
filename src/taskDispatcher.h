#pragma once

#include <vector>
#include <functional>
#include <string>
#include <Arduino.h>
#include <map>
#include <mutex>

// Предварительное объявление
struct TaskArguments;

// Глобальный вектор задач
extern std::vector<TaskArguments> tasks;
extern std::vector<TaskArguments> userTasks;
extern std::mutex tasksMutex;

enum TaskType
{
    SYSTEM,
    DESKTOP,
    USER
};

enum TaskPriority
{
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_CRITICAL = 3
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
    unsigned long lastRunTime;
    unsigned long interval;     // Интервал в тиках (1 тик = 1 мс)
    unsigned long nextRunTime;
    unsigned long executionTime; // Время выполнения в микросекундах
    bool isRunning;              // Флаг выполнения задачи
};

class TaskDispatcher
{
public:
    TaskDispatcher();
    ~TaskDispatcher();

    // Управление задачами
    int sizeTasks();
    void addTask(const TaskArguments &task);
    bool removeTask(const String &taskName);
    bool removeTaskByIndex(const int index);
    bool activateTask(const String &taskName);
    bool deactivateTask(const String &taskName);
    void clearAllTasks();
    void addTasksForSystems();

    // Основной цикл диспетчера
    void tick();
    bool terminal();

    // Управление таймером
    static void initHardwareTimer();
    static void stopHardwareTimer();
    static unsigned long getHardwareTicks();
    static void resetHardwareTicks();

    // Статистика
    int getCPULoad();
    void updateTaskStatistics(const String& taskName, unsigned long executionTime);
    void resetStatistics();

    // Управление вытеснением
    void setTaskBudget(unsigned long budgetMicroseconds);
    unsigned long getTaskBudget() const;
    void setSchedulerQuantum(unsigned long quantumMicroseconds);
    unsigned long getSchedulerQuantum() const;

private:
    // Таймер
    static volatile unsigned long hardwareTicks;
    static esp_timer_handle_t systemTimer;
    static const unsigned long TIMER_INTERVAL_US = 1000; // 1 мс

    // Диспетчеризация
    unsigned long lastTickTime = 0;
    unsigned long totalExecutionTime = 0;
    unsigned long measurementStartTime = 0;
    static const unsigned long MEASUREMENT_WINDOW = 1000;

    // Вытеснение
    unsigned long taskBudget = 5000;    // 5 мс бюджет на задачу
    unsigned long schedulerQuantum = 1000; // 1 мс квант времени

    // Статистика задач
    struct TaskStats
    {
        unsigned long totalExecutionTime;
        unsigned long callCount;
        unsigned long lastExecutionTime;
        unsigned long maxExecutionTime;
        unsigned long minExecutionTime;
    };

    std::map<String, TaskStats> taskStatistics;
    std::mutex statsMutex;

    // Вспомогательные методы
    void executeTask(TaskArguments& task);
    bool shouldRunTask(const TaskArguments& task, unsigned long currentTick);
    void updateTaskSchedule(TaskArguments& task, unsigned long currentTick);

    // Дружественная функция для доступа к private членам
    friend void timerCallback(void *arg);
};

// Глобальные функции
void nullFunction();
void runExFormStack();
void runTasksCore(); // Объявление для terminal()