#include <Arduino.h>
#include "OTAWebUpdater.h"

// Создание экземпляра
OTAWebUpdater otaUpdater;

void setup() {
    Serial.begin(9600);
    Serial.println("ESP32 OTA Web Updater");
    
    // Инициализация с WiFi
    const char* ssid = "Allowed-2g";
    const char* password = "Serjant1985";
    const char* hostname = "ESP32-Firmware";
    
    if (otaUpdater.begin(ssid, password, hostname)) {
        Serial.println("OTA updater initialized successfully");
        
        // Опционально: установка пароля
        // otaUpdater.setPassword("admin123");
        
        // Запуск веб-сервера
        otaUpdater.startWebServer(80);
        
        Serial.println("Open browser and navigate to:");
        Serial.println("http://" + WiFi.localIP().toString());
        Serial.println("or http://" + String(hostname) + ".local");
    } else {
        Serial.println("OTA updater initialization failed");
        Serial.println("Status: " + otaUpdater.getLastStatus());
    }
}

void loop() {
    // Обработка OTA запросов
    otaUpdater.handleClient();
    
    // Ваш основной код здесь
    // ...
    // Serial.println("Updater!!!");
    
    delay(10);
}