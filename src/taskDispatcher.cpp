#pragma once

#include <algorithm>
#include "taskDispatcher.h"
#include "ex.h"
#include "task.h"
#include "systems.h"


// Определение глобального вектора
std::vector<TaskArguments> tasks;
std::vector<TaskArguments> userTasks;


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

// void runExFormStack()
// {
//     if (!formsStack.empty())
//     {
//         exForm *currentForm = formsStack.top();

//         int result = currentForm->showForm();

//         /*
//         The form's "Close" button returns 1
//         */

//         if (result == 1)
//         {
//             formsStack.pop();
//             delete currentForm;

//             delay(250);
//         }
//     }
//     if (formsStack.empty()) _myOSstartupForm();
// }

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

void runTasksCore()
{
    for (TaskArguments &t : tasks)
    {
        if (t.activ && t.f) // проверяем статус и наличие указателя
        {
            // runExFormStack();
            t.f();
        }
    }

    // _SYS.executeAllSystemElements();
}

/* Debug runTaskCore */
// void runTasksCore()
// {
//     for (size_t i = 0; i < tasks.size(); ++i)
//     {
//         auto &t = tasks[i];
//         if (t.activ)
//         {
//             Serial.printf("Task %d: %s, f=%p", i, t.name.c_str(), t.f);
//             if (t.f)
//             {
//                 Serial.println(" -> calling...");
//                 t.f(); // вот здесь падает
//             }
//             else
//             {
//                 Serial.println(" -> NULL function pointer! Skip.");
//             }
//         }
//     }
// }


bool TaskDispatcher::terminal()
{
    _GRF.render(runTasksCore);
    return true;
}

void TaskDispatcher::addTasksForSystems()
{
    for (TaskArguments &t : system0)
    {
        tasks.push_back(t);
    }
}

void nullFunction(){};

/* Debug terminal */
// bool TaskDispatcher::terminal()
// {
//     Serial.printf("Total tasks: %d\n", tasks.size());
//     for (size_t i = 0; i < tasks.size(); ++i) {
//         Serial.printf("Task %d: %s, active: %d, func: %p\n", 
//                      i, tasks[i].name.c_str(), tasks[i].activ, tasks[i].f);
//     }
    
//     _GRF.render(runTasksCore);
//     return true;
// }