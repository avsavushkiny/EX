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
    void clearExFormsStack();
    void addTasksForSystems();
    bool terminal();
    void tick();  // Основной тик диспетчера задач

    // Метод для расчета загрузки процессора
    int getCPULoad();
    // Метод для сбора статистики выполнения задач
    void updateTaskStatistics(const String& taskName, unsigned long executionTime);

    // Методы аппаратного таймера таймера
    static void initHardwareTimer();
    static void stopHardwareTimer();
    static unsigned long getHardwareTicks();
    
private:
    unsigned long systemTicks = 0;
    unsigned long lastTickRealTime = 0;    // Реальное время последнего тика
    // unsigned long totalExecutionTime = 0;  // Время выполнения задач за период (мкс)
    // unsigned long measurementStartTime = 0;// Начало периода измерения (мс)
    
    // Константы для расчета
    // static const unsigned long MEASUREMENT_WINDOW = 1000; // Окно измерения 1 секунда

    // Структура для статистики по задачам
    struct TaskStats
    {
        unsigned long totalExecutionTime;
        unsigned long callCount;
        unsigned long lastExecutionTime;
    };
    
    std::map<String, TaskStats> taskStatistics;
    
protected:
    unsigned long measurementStartTime = 0; // Начало периода измерения (мс)
    static const unsigned long MEASUREMENT_WINDOW = 1000; // Окно измерения 1 секунда
    unsigned long totalExecutionTime = 0;  // Время выполнения задач за период (мкс)
};

// Структура для хранения контекста задачи (для псевдо-вытесняющей многозадачности)
struct TaskContext {
    unsigned long lastYieldTime;
    unsigned long maxExecutionTime;  // Максимальное время выполнения в мкс
    unsigned long totalExecutionTime;
    unsigned int yieldCount;
    bool needsYield;
};

// Класс планировщика с поддержкой разных стратегий
class TaskScheduler {
public:
    enum ScheduleStrategy {
        ROUND_ROBIN,      // Циклическое переключение
        PRIORITY_BASED,   // На основе приоритетов
        TIME_SLICING,     // Квантование времени
        COOPERATIVE       // Чисто кооперативная (текущая)
    };
    
    void setStrategy(ScheduleStrategy strategy);
    void setTimeSlice(unsigned long microseconds);  // Квант времени для Time Slicing
    void yield();  // Добровольная передача управления
    
protected:
    ScheduleStrategy currentStrategy = COOPERATIVE;
    unsigned long timeSlice = 5000;  // 5 мс по умолчанию
    int currentTaskIndex = 0;
};

// Расширенная версия TaskDispatcher с многозадачностью
class MultiTaskingDispatcher : public TaskDispatcher {
public:
    void tick();  // Переопределяем tick
    void runMultiTasking();  // Основной цикл многозадачности
    void yield();  // Для добровольной передачи управления из задач
    
    // Управление задачами
    bool suspendTask(const String &taskName);
    bool resumeTask(const String &taskName);
    bool setTaskPriority(const String &taskName, TaskPriority priority);
    
    // Настройка планировщика
    void setSchedulerStrategy(TaskScheduler::ScheduleStrategy strategy);
    void setTimeSlice(unsigned long microseconds);
    
private:
    TaskScheduler scheduler;
    std::map<String, TaskContext> taskContexts;
    std::vector<TaskArguments*> activeTasks;  // Кэш активных задач
    unsigned long lastSchedulerRun = 0;
    
    void updateActiveTasksCache();
    void executeTask(TaskArguments &task);
    bool checkTaskTimeout(const String &taskName, unsigned long startTime);
};

// Глобальный экземпляр многозадачного диспетчера
extern MultiTaskingDispatcher MTD;