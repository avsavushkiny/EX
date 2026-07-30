#include <Arduino.h>
#include "OTAWebUpdater.h"

OTAWebUpdater otaUpdater;

void setup()
{
    Serial.begin(115200);
    Serial.println("\n");
    Serial.println("ESP32 OTA Updater - Access Point Mode");

    // Запуск как точка доступа
    // Параметры: SSID, пароль (опционально, минимум 8 символов)
    if (otaUpdater.begin("ESP32-Update", "password123"))
    {
        Serial.println("Access Point started successfully");

        // Запуск веб-сервера
        otaUpdater.startWebServer(80);

        Serial.println("\n=== CONNECTION INFO ===");
        Serial.println("Connect to WiFi: ESP32-Update");
        Serial.println("Password: password123");
        Serial.print("Open browser: http://");
        Serial.println(otaUpdater.getLocalIP());
        Serial.println("========================");
    }
    else
    {
        Serial.println("Failed to start Access Point");
    }
}

void loop()
{
    // Обработка OTA запросов
    otaUpdater.handleClient();

    // Ваш основной код здесь
    // ...

    delay(10);
}