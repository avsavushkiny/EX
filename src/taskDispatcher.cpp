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

void runExFormStack()
{
    if (!formsStack.empty())
    {
        exForm *currentForm = formsStack.top();

        int result = currentForm->showForm();

        /*
        The form's "Close" button returns 1
        */

        if (result == 1)
        {
            formsStack.pop();
            delete currentForm;

            delay(250);
        }
    }
    if (formsStack.empty()) _myOSstartupForm();
}

void runTasksCore()
{
    for (TaskArguments &t : tasks)
    {
        if (t.activ)
        {
            runExFormStack();
            t.f();
        }
    }

    _SYS.executeAllSystemElements();
}

bool TaskDispatcher::terminal()
{
    _GRF.render(runTasksCore);
}