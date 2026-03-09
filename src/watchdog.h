#pragma once

struct RunningTaskInfo
{
    String name;
    unsigned long startTime;
    bool isActive;
};

extern RunningTaskInfo runningTaskInfo;
extern void taskWatchdogOnCore1(void *parameter);