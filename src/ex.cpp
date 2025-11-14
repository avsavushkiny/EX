#include "ex.h"

// Global object (объявляем как extern)
Graphics _GRF;
TaskDispatcher _TD;
GGL _GGL;
Icon _ICON;
SystemIcon _SICON;
Joystick _JOY;
Cursor _CRS;

/* Global function */
/* Initialization systems */
void initializationSystem()
{
    /* 
       GPIO release from sleep

       esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 1);
       esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 1); 
       esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); // Stick 0
       esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1); // EX button
    */
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1);    // Stick 0
    
    /* setting the operating system state */
    Serial.begin(9600);
    _GGL.gray.begin();

    /* setting the resolution of the analog-to-digital converter */
    analogReadResolution(RESOLUTION_ADC);

    /* determine the backlight-port mode */
    pinMode(PIN_BACKLIGHT_LCD, OUTPUT);;
    
    /* determine the operating modes of digital ports */
    pinMode(PIN_BUTTON_ENTER, INPUT);
    pinMode(PIN_BUTTON_EX,    INPUT);
    pinMode(PIN_BUTTON_A,     INPUT);
    pinMode(PIN_BUTTON_B,     INPUT);
    pinMode(PIN_BATTERY,      INPUT);

    /* Contrast & run system element */
    // systems.setDisplayContrast(240);
    // systems.executeAllSystemElements();
    

    /*
       Vector
       moving system-task to the vector
       determine the number of tasks in the vector
    */
    _TD.addTasksForSystems();

    // Clear buffer LCD display
    _GGL.gray.clearBuffer(); // -->

    // output text
    String text = "SozvezdiyeOS ";
    _GGL.gray.writeLine(5, 123, text + VERSION_EX, 10, 1, _GGL.gray.BLACK);
    
    // draw gray-line
    _GGL.gray.drawHLine(0, 135, 256, _GGL.gray.LIGHT_GRAY, 5);
    _GGL.gray.drawHLine(0, 140, 256, _GGL.gray.DARK_GRAY, 5);
    _GGL.gray.drawHLine(0, 145, 256, _GGL.gray.BLACK, 5);

    // send data to display
    _GGL.gray.sendBuffer(); // <--

    delay(2500);
}