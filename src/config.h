#pragma once

#include <stdint.h>

/* MOSAIC — Microcontroller OS And Introductory Coding.
   Аббревиатура, которая складывается в слово "мозаика" — идея о том,
   что программист собирает программу из маленьких кусочков кода. */

const String _NAME_OS = "MOSAIC";
const String _VERSION_CORE = "0.2.0";

// LCD resolution
const int H_LCD = 160;
const int W_LCD = 256;

// ADC resolution
const int8_t RESOLUTION_ADC = 12;

// Pin definitions
const int8_t PIN_STICK_0X = 33;
const int8_t PIN_STICK_0Y = 32;
const int8_t PIN_STICK_1Y = 34;
const int8_t PIN_STICK_1X = 35;
const int8_t PIN_BUTTON_ENTER = 27;
const int8_t PIN_BUTTON_EX = 14;
const int8_t PIN_BUTTON_A = 12;
const int8_t PIN_BUTTON_B = 13;
const int8_t PIN_BACKLIGHT_LCD = 0;
const int8_t PIN_BUZZER = 26;
const int8_t PIN_BATTERY = 39;