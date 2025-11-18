#pragma once

#include <algorithm>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"


// Определение глобального вектора
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;

static unsigned long taskStartTime = 0;
static String currentTaskName = "";


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

void TaskDispatcher::addTasksForSystems()
{
    for (TaskArguments &t : system0)
    {
        tasks.push_back(t);
    }
}

// Без статистики
// void TaskDispatcher::tick()
// {
//     systemTicks++;
    
//     // Сортируем задачи по приоритету (критические задачи выполняются первыми)
//     std::vector<TaskArguments*> sortedTasks;
//     for (auto &task : tasks)
//     {
//         if (task.activ && task.f)
//         {
//             sortedTasks.push_back(&task);
//         }
//     }
    
//     // Сортировка по приоритету (от высокого к низкому)
//     std::sort(sortedTasks.begin(), sortedTasks.end(), 
//               [](const TaskArguments* a, const TaskArguments* b) {
//                   return a->priority > b->priority;
//               });
    
//     // Выполняем задачи в порядке приоритета
//     for (auto taskPtr : sortedTasks)
//     {
//         auto &task = *taskPtr;
        
//         // Проверяем, нужно ли выполнять задачу в этом тике
//         if (systemTicks >= task.nextRunTime)
//         {
//             // Выполняем задачу
//             if (task.f)
//             {
//                 task.f();
//             }
            
//             task.lastRunTime = systemTicks;
            
//             // Обновляем время следующего выполнения
//             if (task.interval > 0)
//             {
//                 task.nextRunTime = systemTicks + task.interval;
//             }
//             else
//             {
//                 task.nextRunTime = systemTicks + 1; // Минимальный интервал
//             }
            
//             // Если задача одноразовая, деактивируем её
//             if (task.oneShot)
//             {
//                 task.activ = false;
//             }
//         }
//     }
// }

// Со статистикой
void TaskDispatcher::tick()
{
    systemTicks++;
    
    // Сортируем задачи по приоритету (критические задачи выполняются первыми)
    std::vector<TaskArguments*> sortedTasks;
    for (auto &task : tasks)
    {
        if (task.activ && task.f)
        {
            sortedTasks.push_back(&task);
        }
    }
    
    // Сортировка по приоритету (от высокого к низкому)
    std::sort(sortedTasks.begin(), sortedTasks.end(), 
              [](const TaskArguments* a, const TaskArguments* b) {
                  return a->priority > b->priority;
              });
    
    // Выполняем задачи в порядке приоритета
    for (auto taskPtr : sortedTasks)
    {
        auto &task = *taskPtr;
        
        // Проверяем, нужно ли выполнять задачу в этом тике
        if (systemTicks >= task.nextRunTime)
        {
            // Измеряем время выполнения задачи
            unsigned long startTime = micros();
            currentTaskName = task.name;
            
            // Выполняем задачу
            if (task.f)
            {
                task.f();
            }
            
            // Рассчитываем время выполнения
            unsigned long endTime = micros();
            unsigned long executionTime = endTime - startTime;
            
            // Обновляем статистику (переводим в миллисекунды)
            updateTaskStatistics(task.name, executionTime / 1000);
            
            task.lastRunTime = systemTicks;
            
            // Обновляем время следующего выполнения
            if (task.interval > 0)
            {
                task.nextRunTime = systemTicks + task.interval;
            }
            else
            {
                task.nextRunTime = systemTicks + 1; // Минимальный интервал
            }
            
            // Если задача одноразовая, деактивируем её
            if (task.oneShot)
            {
                task.activ = false;
            }
        }
    }
    
    // Периодически сбрасываем статистику (каждые 10 секунд)
    if (systemTicks % 10000 == 0) {
        totalExecutionTime = 0;
        taskStatistics.clear();
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
    _TD.tick(); // Используем новый тиковый диспетчер
    // runExFormStack();
#else
    Serial.printf("Total tasks: %d\n", tasks.size());
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, priority: %d, oneShot: %d, interval: %lu\n",
                      i, tasks[i].name.c_str(), tasks[i].activ, 
                      tasks[i].priority, tasks[i].oneShot, tasks[i].interval);
    }

    _TD.tick(); // Используем новый тиковый диспетчер
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
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        Serial.printf("Task %d: %s, active: %d, func: %p\n",
                      i, tasks[i].name.c_str(), tasks[i].activ, tasks[i].f);
    }

    _GRF.render(runTasksCore);
    return true;
#endif
}

void nullFunction(){};

// Статистика
void TaskDispatcher::updateTaskStatistics(const String& taskName, unsigned long executionTime)
{
    if (taskStatistics.find(taskName) == taskStatistics.end()) {
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
    unsigned long windowSize = currentTime - lastMeasurementTime;
    
    // Если окно измерения слишком маленькое, возвращаем предыдущее значение
    if (windowSize < 100) {
        // Возвращаем последнее рассчитанное значение или 0
        static int lastLoad = 0;
        return lastLoad;
    }
    
    // Рассчитываем загрузку CPU в процентах
    // totalExecutionTime - суммарное время выполнения задач за период
    // windowSize - длительность периода измерения
    
    int cpuLoad = 0;
    if (windowSize > 0) {
        cpuLoad = (totalExecutionTime * 100) / windowSize;
        
        // Ограничиваем значение 0-100%
        if (cpuLoad > 100) cpuLoad = 100;
        if (cpuLoad < 0) cpuLoad = 0;
    }
    
    // Сбрасываем статистику для нового периода измерения
    totalExecutionTime = 0;
    lastMeasurementTime = currentTime;
    
    return cpuLoad;
}