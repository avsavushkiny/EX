#include "ex.h"

// Global object (объявляем как extern)
Graphics _GRF;
TaskDispatcher _TD;
GGL _GGL;
Icon _ICON;
SystemIcon _SICON;
Joystick _JOY;
Cursor _CRS;
OTAWebUpdater _OTA_UPDATER;
WiFiManager wifiManager;

/* Global function */
/* Initialization systems */
void initializationSystem()
{
   /* Инициализация аппоратного таймера */
   _TD.initHardwareTimer();

   // wifiManager.begin();
   // wifiManager.setDebug(false);

   // Создание задачи для второго ядра
   // #ifndef WATCHDOG
   // #else
   // xTaskCreatePinnedToCore(
   //     taskWatchdogOnCore1,
   //     "TaskWatchdog",
   //     4096,
   //     nullptr,
   //     20,
   //     nullptr,
   //     1 // Ядро 1
   // );
   // #endif

   /*
      Vector
      moving system-task to the vector
      determine the number of tasks in the vector
   */
   _TD.addTasksForSystems();
}