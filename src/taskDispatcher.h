#pragma once
#include <vector>
#include <string>
#include <Arduino.h>

// Глобальный вектор задач
extern std::vector<TaskArguments> tasks;

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
    int sizeTasks();
    void addTask(const TaskArguments &task);
    bool removeTaskVector(const String &taskName);
    bool removeTask(const String &taskName);
    bool removeTaskIndex(const int index);
    bool runTask(const String &taskName);
    bool terminal();
};