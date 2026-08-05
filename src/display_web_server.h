#ifndef DISPLAY_WEB_SERVER_H
#define DISPLAY_WEB_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <vector>

class DisplayWebServer
{
public:
    DisplayWebServer();
    ~DisplayWebServer();

    // Инициализация как WiFi точка доступа
    bool begin(const char *apSSID = "Display-Server", const char *apPassword = "");
    
    // Запуск веб-сервера
    void startWebServer(int port = 80);
    
    // Обработка клиентов
    void handleClient();
    
    // Установка буфера дисплея
    void setDisplayBuffer(uint8_t *buffer, int width, int height);
    
    // Получение статуса и IP
    String getLastStatus() const;
    IPAddress getLocalIP() const;

private:
    WebServer *_server;
    String _apSSID;
    String _lastStatus;
    IPAddress _localIP;
    
    // Указатель на буфер дисплея
    uint8_t *_displayBuffer;
    int _width;
    int _height;
    bool _hasBuffer;
    
    // Генерация HTML страницы
    String getDisplayPageHTML();
    
    // Обработчики запросов
    void handleRoot();
    void handleDisplayImage();
    void handleStatus();
    void handleNotFound();
    
    // Конвертация буфера в BMP
    std::vector<uint8_t> convertToBMP();
    
    // Вспомогательные функции
    String getBytes(uint64_t bytes);
};

#endif