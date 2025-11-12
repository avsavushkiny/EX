#pragma once

#include <Arduino.h>
#include "config.h"
#include "ggl.h"
#include "taskDispatcher.h"
#include "graphics.h"
#include "icon.h"
#include "ui.h"
#include "input.h"

/* [!] Объекты в объявлении через extern 
       не должны содержать инициализацию! */

// Глобальные объекты (объявляем как extern)
extern Graphics _GRF;
extern TaskDispatcher _TD;
extern GGL _GGL;
extern Icon _ICON;
extern SystemIcon _SICON;
extern Joystick _JOY;

// Глобальные функции
extern void initializationSystem();