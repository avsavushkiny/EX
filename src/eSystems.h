#pragma once

#include <Arduino.h>
#include "config.h"
#include "ggl.h"
#include "input.h"
#include "ui.h"
#include "ex.h"

/* [!] Используем форвард-декларацию, что бы
       компилятор видел класс Systems */
class Systems; // Forward declaration

extern GGL _GGL;
extern Joystick _JOY;

// Абстрактный класс системных элементов
class eSystemElement
{
public:
    virtual void execute() = 0;          // Чисто виртуальный метод
    virtual ~eSystemElement() = default; // Виртуальный деструктор
};
/* Управление подстветкой дисплея */
class eBacklight : public eSystemElement
{
public:
    eBacklight(bool stateLight) : m_stateLight(stateLight) {}

    // Устанавливаем подсветку дисплея
    void execute() override;

    // Устанавливаем новое значение подсветки дисплея
    void setBacklight(const bool newState)
    {
        m_stateLight = newState;
    }
    // Получаем значение статуса подсветки дисплея
    bool getBacklight() const
    {
        return m_stateLight;
    }

private:
    bool m_stateLight;
};
/* Управление контрастностью дисплея */
class eDisplayContrast : public eSystemElement
{
public:
    eDisplayContrast(int value) : m_valueContrast(value) {}

    void execute() override;

    void setDisplayContrast(int newValue)
    {
        m_valueContrast = newValue;
    }

    int getDisplayContrast()
    {
        return m_valueContrast;
    }

private:
    int m_valueContrast = 143;
};
/* Управление режимами энергосбережения*/
class ePowerSave : public eSystemElement
{
public:
    ePowerSave(bool statePowerSave) : m_statePowerSave(statePowerSave) {}

    void setSystem(Systems* sys) { m_system = sys; }
    
    bool isTouched();
    void execute() override;

private:
    bool m_statePowerSave;
    Systems* m_system = nullptr;
    // for screensaver
    unsigned long screenTiming{}, screenTiming2{}, TIMER{};
};
/* Управление курсором */
class eCursor : public eSystemElement
{
public:
    eCursor(bool stateCursor) : m_stateCursor(stateCursor) {}

    void execute() override
    {
        _JOY.updatePositionXY(20);
        _CRS.cursor(true, _JOY.posX0, _JOY.posY0);
    }

private:
    bool m_stateCursor;
};
/* Управление портом данных */
class eDataPort : public eSystemElement
{
public:
    eDataPort(bool stateDataPort, int port) : m_stateDataPort(stateDataPort), m_port(port) {}

    void execute() override;

private:
    bool m_stateDataPort;
    int m_port;
};