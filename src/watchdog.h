#pragma once

struct RunningTaskInfo
{
    String name;
    unsigned long startTime;
    bool isActive;
};

// #ifndef WATCHDOG
// #else
extern RunningTaskInfo runningTaskInfo;
extern void taskWatchdogOnCore1(void *parameter);
// #endif