#include "input.h"

bool Joystick::pressKeyENTER()
{
    if (digitalRead(PIN_BUTTON_ENTER) == true)
    {
        return true;
    }
    else return false;
}

bool Joystick::pressKeyEX()
{
    if (digitalRead(PIN_BUTTON_EX) == true)
    {
        return true;
    }
    else return false;
}

bool Joystick::pressKeyA()
{
    if (digitalRead(PIN_BUTTON_A) == true)
    {
        return true;
    }
    else return false;
}

bool Joystick::pressKeyB()
{
    if (digitalRead(PIN_BUTTON_B) == true)
    {
        return true;
    }
    else return false;
}

int Joystick::calculatePositionY0() // 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    // if (RAW_DATA_Y0 < 0 || RAW_DATA_Y0 > 1023)
    // {
    //     return COOR_Y0; // Игнорируем ошибочные данные
    // }

    // Serial.print("Raw data Y: "); Serial.println(RAW_DATA_Y0);
    
    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 600)) /*&& (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100))*/)
    {
        COOR_Y0 += 2;
        if (COOR_Y0 >= 160)
            COOR_Y0 = 160;

        return COOR_Y0;
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 600)) /*&& (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100))*/)
    {
        COOR_Y0 -= 2;
        if (COOR_Y0 <= 0)
            COOR_Y0 = 0;

        return COOR_Y0;
    }
    else return COOR_Y0;
}

int Joystick::calculatePositionX0() // 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    // if (RAW_DATA_X0 < 0 || RAW_DATA_X0 > 1023) 
    // {
    //     return COOR_X0; // Игнорируем ошибочные данные
    // }
    // Serial.print("Raw data X: "); Serial.println(RAW_DATA_X0);
    
    if ((RAW_DATA_X0 < (DEF_RES_X0 - 600)) /*&& (RAW_DATA_X0 > (DEF_RES_X0 - 1200))*/)
    {
        COOR_X0 -= 2;
        if (COOR_X0 <= 0)
            COOR_X0 = 0;

        return COOR_X0;
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 600)) /*&& (RAW_DATA_X0 < (DEF_RES_X0 + 1100))*/)
    {
        COOR_X0 += 2;
        if (COOR_X0 >= 256)
            COOR_X0 = 256;

        return COOR_X0;
    }
    else return COOR_X0;
}

void Joystick::resetPositionXY()
{
    COOR_X0 = 128;
    COOR_X1 = 128;
    COOR_Y0 = 80;
    COOR_Y1 = 80;
}

void Joystick::updatePositionXY(uint delay)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= delay)
    {
        prevTime = currTime;

        posX0 = calculatePositionX0();
        posY0 = calculatePositionY0();

        indexX0 = calculateIndexX0();
        indexY0 = calculateIndexY0();
    }
}

void Joystick::updatePositionXY()
{
    posX0 = calculatePositionX0();
    posY0 = calculatePositionY0();

    indexX0 = calculateIndexX0();
    indexY0 = calculateIndexY0();
}

int8_t Joystick::calculateIndexY0() // obj 0y
{
    RAW_DATA_Y0 = analogRead(PIN_STICK_0Y);

    if ((RAW_DATA_Y0 < (DEF_RES_Y0 - 500)) && (RAW_DATA_Y0 > (DEF_RES_Y0 - 1100)))
    {
        return OBJ_Y0 = OBJ_Y0 - 1;
    }
    else if (RAW_DATA_Y0 < (DEF_RES_Y0 - 1100))
    {
        return OBJ_Y0 = OBJ_Y0 - 1; // 2
    }
    else if ((RAW_DATA_Y0 > (DEF_RES_Y0 + 500)) && (RAW_DATA_Y0 < (DEF_RES_Y0 + 1100)))
    {
        return OBJ_Y0 = OBJ_Y0 + 1;
    }
    else if (RAW_DATA_Y0 > (DEF_RES_Y0 + 1100))
    {
        return OBJ_Y0 = OBJ_Y0 + 1; // 2
    }
    else return OBJ_Y0 = 0;
}

int8_t Joystick::calculateIndexX0() // obj 0x
{
    RAW_DATA_X0 = analogRead(PIN_STICK_0X);

    if ((RAW_DATA_X0 < (DEF_RES_X0 - 500)) && (RAW_DATA_X0 > (DEF_RES_X0 - 1100)))
    {
        return OBJ_X0 = OBJ_X0 - 1;
    }
    else if (RAW_DATA_X0 < (DEF_RES_X0 - 1100))
    {
        return OBJ_X0 = OBJ_X0 - 1; // 2
    }
    else if ((RAW_DATA_X0 > (DEF_RES_X0 + 500)) && (RAW_DATA_X0 < (DEF_RES_X0 + 1100)))
    {
        return OBJ_X0 = OBJ_X0 + 1;
    }
    else if (RAW_DATA_X0 > (DEF_RES_X0 + 1100))
    {
        return OBJ_X0 = OBJ_X0 + 1; // 2
    }
    else return OBJ_X0 = 0;
}