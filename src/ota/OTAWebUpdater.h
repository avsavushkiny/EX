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

    // Инициализация OTA сервера
    bool begin(const char* ssid, const char* password, const char* hostname = "ESP32-OTA");
    
    // Запуск веб-сервера
    void startWebServer(int port = 80);
    
    // Обработка клиентов (должна вызываться в loop)
    void handleClient();
    
    // Проверка наличия обновлений (можно использовать для автоматических проверок)
    bool checkForUpdate();
    
    // Статус последней операции
    String getLastStatus() const;
    
    // Настройка пароля для защиты веб-интерфейса (опционально)
    void setPassword(const char* password);

private:
    WebServer* _server;
    String _hostname;
    String _lastStatus;
    String _password;
    bool _passwordProtected;
    
    // HTML страница для загрузки прошивки
    String getUpdatePageHTML();
    
    // Обработчики веб-запросов
    void handleRoot();
    void handleUpdate();
    void handleNotFound();
    void handleFirmwareUpload();
    void handleReset();
    
    // Вспомогательные функции
    String getContentType(String filename);
    String getBytes(uint64_t bytes);
    
    // Обработка загрузки
    bool uploadFirmware(WebServer& server);
};

#endif