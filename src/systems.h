#pragma once

class Systems;
extern Systems _SYS;

#include "eSystems.h"

class Systems
{
public:
    // Конструктор, инициализирующий все подсистемы
    Systems(bool backlightState, int valueContrast, bool powerSaveState, bool cursorState, bool dataPortState, int port)
        : backlight(backlightState), displayContrast(valueContrast), powerSave(powerSaveState), cursor(cursorState), dataPort(dataPortState, port) {
            powerSave.setSystem(this);
        }

    // Методы для управления lcd
    bool STATEBACKLIGHT;
    void setBacklight(bool newState)
    {
        backlight.setBacklight(newState);
    }
    bool getBacklight() const
    {
        return backlight.getBacklight();
    }

    // Методы установки контрастности дисплея
    int VALUECONTRAST{240}; // 143 old value
    void setDisplayContrast(int newValue)
    {
        displayContrast.setDisplayContrast(newValue);
    }
    int getDisplayContrast()
    {
        return displayContrast.getDisplayContrast();
    }

    // Методы для выполнения задач всех подсистем
    void executeAllSystemElements()
    {
        // backlight.execute();
        // displayContrast.execute();
        // powerSave.execute();
        cursor.execute();
        // dataPort.execute();
    }

private:
    eBacklight backlight{true};             // Управление подсветкой
    eDisplayContrast displayContrast{143};  // Установка контрастности дисплея
    ePowerSave powerSave{true};             // Управление режимами энергосбережения
    eCursor cursor{true};                   // Управление курсором
    eDataPort dataPort{true, 0};            // Управление портом данных
};