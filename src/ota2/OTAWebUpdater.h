#ifndef OTAWebUpdater_h
#define OTAWebUpdater_h

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

class OTAWebUpdater {
public:
    OTAWebUpdater();
    ~OTAWebUpdater();

    // Инициализация как WiFi точка доступа
    bool begin(const char* apSSID = "EX-OTA", const char* apPassword = "");
    
    // Запуск веб-сервера
    void startWebServer(int port = 80);
    
    // Обработка клиентов (должна вызываться в loop)
    void handleClient();
    
    // Получение статуса
    String getLastStatus() const;
    
    // Получение IP адреса
    IPAddress getLocalIP() const;

private:
    WebServer* _server;
    String _apSSID;
    String _lastStatus;
    IPAddress _localIP;
    
    // HTML страница в стиле Microsoft
    String getUpdatePageHTML();
    
    // Обработчики веб-запросов
    void handleRoot();
    void handleUpdate();
    void handleFirmwareUpload();
    void handleStatus();
    void handleNotFound();
    
    // Вспомогательные функции
    String getBytes(uint64_t bytes);
};

#endif