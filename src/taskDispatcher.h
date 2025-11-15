#pragma once

#include <vector>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Предварительное объявление
struct TaskArguments;

// Глобальный вектор задач
extern std::vector<TaskArguments> tasks;
extern std::vector<TaskArguments> userTasks;
extern SemaphoreHandle_t taskMutex;

enum TaskType
{
    SYSTEM,
    DESKTOP,
    USER
};

struct TaskArguments
{
    String name;
    void (*f)(void);
    const uint8_t *bitMap;
    TaskType type;
    int index;
    bool activ;
};

class TaskDispatcher
{
public:
    static void init();                      // инициализация мьютекса
    int sizeTasks();
    void addTask(const TaskArguments &task);
    bool removeTaskVector(const String &taskName);
    bool removeTask(const String &taskName);
    bool removeTaskIndex(const int index);
    bool runTask(const String &taskName);
    void addTasksForSystems();
    bool terminal(); // запускает FreeRTOS-задачу
};
