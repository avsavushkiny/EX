#pragma once

#include <vector>
#include <functional>
#include <string>
#include <Arduino.h>

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

struct TaskArguments
{
    String name;
    // std::function<void()> f;
    void (*f)(void);
    const uint8_t *bitMap;
    TaskType type;
    int index;
    bool activ;
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
};