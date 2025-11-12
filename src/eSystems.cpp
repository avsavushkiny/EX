#include "eSystems.h"
#include "systems.h"

/* eBacklight */
void eBacklight::execute()
{
    //p-n-p transistor
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);

    if (m_stateLight == true)
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 0); // on
    }
    else
    {
        digitalWrite(PIN_BACKLIGHT_LCD, 1); // off
    }
}
/* eDisplayContrast */
void eDisplayContrast::execute()
{
    // u8g2.setContrast(m_valueContrast);!!!
    // Serial.println(m_valueContrast);
    _GGL.gray.setContrast(m_valueContrast);
}
/* ePowerSave */
bool ePowerSave::isTouched()
{
    if ((_JOY.calculateIndexY0() == 0) && (_JOY.calculateIndexX0() == 0)) return false;

    return true;
}
/**/
void ePowerSave::execute()
{
    if (!m_system) return;
    
    String text = "The console has entered\nhibernation mode.\n\nTo exit this mode,\nmove the stick upwards.";
    
    _GGL.gray.writeLine(17, 150, "SLEEP LINE", 10, 1, _GGL.gray.LIGHT_GRAY);
    
    if (isTouched() == true)
    {
        screenTiming = TIMER;
        _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
    }
    
    if ((TIMER - screenTiming > 60000) && (_JOY.posY0 >= 150))
    {
        screenTiming = TIMER;

        while (isTouched() == false)
        {
            InstantMessage message1("The console has entered\ndeep sleep mode.", 3000);
            message1.show();

            m_system->setBacklight(false); // <-- было _SYS.setBacklight
            _GGL.gray.setPowerMode(_GGL.gray.SLEEP_MODE);
            esp_deep_sleep_start();
        }
    }
    
    if ((TIMER - screenTiming > 60000) && (_JOY.posY0 < 150))
    {
        screenTiming = TIMER;

        while (isTouched() == false)
        {
            InstantMessage message2(text, 3000);
            message2.show();

            m_system->setBacklight(false); // <-- было _SYS.setBacklight
            _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
            esp_light_sleep_start();
        }
    }
}

/* eCursor */
void eCursor::execute()
{

}
/* eDataPort */
void eDataPort::execute()
{

}