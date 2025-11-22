#pragma once

#include <vector>
#include <functional>
#include <string>
#include <Arduino.h>
#include <map>

// Предварительное объявление
struct TaskArguments;

// Глобальный вектор задач
extern std::vector<TaskArguments> tasks;
extern std::vector<TaskArguments> userTasks;

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
    TaskPriority priority;      // Приоритет задачи
    bool oneShot;               // Одноразовая задача
    unsigned long lastRunTime;  // Время последнего выполнения
    unsigned long interval;     // Интервал выполнения (в тиках)
    unsigned long nextRunTime;  // Время следующего выполнения
};

class TaskDispatcher
{
public:
    int sizeTasks();
    void addTask(const TaskArguments &task);
    bool removeTaskVector(const String &taskName);
    bool removeTask(const String &taskName);
    bool removeTaskIndex(const int index);
    bool runTask(const String &taskName);
    void addTasksForSystems();
    bool terminal();
    void tick();  // Основной тик диспетчера задач

    // Метод для расчета загрузки процессора
    int getCPULoad();
    // Метод для сбора статистики выполнения задач
    void updateTaskStatistics(const String& taskName, unsigned long executionTime);
    
    // Hardware timer
    static void initHardwareTimer();
    static void stopHardwareTimer();
    static unsigned long getHardwareTicks();
    
private:
    unsigned long systemTicks = 0;
    unsigned long lastTickRealTime = 0;    // Реальное время последнего тика
    unsigned long totalExecutionTime = 0;  // Время выполнения задач за период (мкс)
    unsigned long measurementStartTime = 0;// Начало периода измерения (мс)
    
    // Константы для расчета
    static const unsigned long MEASUREMENT_WINDOW = 1000; // Окно измерения 1 секунда

    // Структура для статистики по задачам
    struct TaskStats
    {
        unsigned long totalExecutionTime;
        unsigned long callCount;
        unsigned long lastExecutionTime;
    };
    
    std::map<String, TaskStats> taskStatistics;
};