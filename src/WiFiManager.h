#ifndef WiFiManager_h
#define WiFiManager_h

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <functional>
#include <HTTPClient.h>

// Типы подключения
enum class WiFiMode
{
    STATION,      // Клиентский режим
    ACCESS_POINT, // Точка доступа
    BOTH          // Оба режима
};

// Статусы подключения
enum class WiFiStatus
{
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    FAILED,
    AP_ACTIVE
};

// Структура для хранения информации о сети
struct NetworkInfo
{
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    String bssid;
    int channel;

    String toString() const
    {
        return ssid + " (" + String(rssi) + " dBm)";
    }
};

// Колбэки для событий
typedef std::function<void(WiFiStatus)> WiFiStatusCallback;
typedef std::function<void(const NetworkInfo &)> NetworkFoundCallback;
typedef std::function<void(const String &)> WiFiMessageCallback;

class WiFiManager
{
public:
    // Конструктор / Деструктор
    WiFiManager();
    ~WiFiManager();

    // ===== ОСНОВНЫЕ ФУНКЦИИ =====

    // Инициализация
    void begin();

    // Подключение к WiFi сети (клиентский режим)
    bool connect(const String &ssid, const String &password = "", int timeout = 20000);

    // Подключение с сохраненными настройками
    bool connectSaved(int timeout = 20000);

    // Отключение от WiFi
    void disconnect();

    // Запуск точки доступа
    bool startAccessPoint(const String &ssid, const String &password = "", int channel = 1, bool hidden = false);

    // Остановка точки доступа
    void stopAccessPoint();

    // Установка режима работы
    void setMode(WiFiMode mode);

    // ===== УПРАВЛЕНИЕ НАСТРОЙКАМИ =====

    // Сохранение учетных данных в NVS
    bool saveCredentials(const String &ssid, const String &password);

    // Загрузка сохраненных учетных данных
    bool loadCredentials(String &ssid, String &password);

    // Очистка сохраненных данных
    void clearCredentials();

    // Проверка наличия сохраненных данных
    bool hasSavedCredentials() const;

    // ===== ИНФОРМАЦИЯ О СОСТОЯНИИ =====

    // Текущий статус
    WiFiStatus getStatus() const;

    // IP адрес
    IPAddress getIP() const;
    IPAddress getSubnet() const;
    IPAddress getGateway() const;
    IPAddress getDNS() const;
    IPAddress getAPIP() const;

    // MAC адреса
    String getMacAddress() const;
    String getAPMacAddress() const;

    // Информация о сети
    String getSSID() const;
    int32_t getRSSI() const;
    uint8_t getSignalQuality() const; // 0-100%

    // Время работы
    unsigned long getUptime() const;

    // ===== СКАНИРОВАНИЕ СЕТЕЙ =====

    // Сканирование доступных сетей
    std::vector<NetworkInfo> scanNetworks(bool async = false);

    // Асинхронное сканирование (должно вызываться в loop)
    bool startAsyncScan();
    bool isScanComplete() const;
    std::vector<NetworkInfo> getScanResults();

    // ===== mDNS =====

    bool startMDNS(const String &hostname);
    void stopMDNS();
    void addMDNSService(const String &service, const String &protocol, uint16_t port);

    // ===== КОЛБЭКИ =====

    // Установка колбэков
    void onStatusChange(WiFiStatusCallback callback);
    void onNetworkFound(NetworkFoundCallback callback);
    void onMessage(WiFiMessageCallback callback);

    // ===== СЛУЖЕБНЫЕ ФУНКЦИИ =====

    // Автоматическое переподключение (вызывать в loop)
    void autoReconnect();

    // Получение форматированной информации
    String getInfoString() const;
    String getStatusString() const;

    // Установка таймаутов
    void setTimeout(int ms);
    void setReconnectInterval(int ms);

    // Включение/отключение логирования
    void setDebug(bool enabled);

private:
    // Состояния
    WiFiStatus _status;
    WiFiMode _mode;
    String _ssid;
    String _password;
    String _apSSID;
    String _apPassword;
    IPAddress _localIP;
    IPAddress _apIP;
    unsigned long _connectStartTime;
    unsigned long _lastReconnectAttempt;
    int _timeout;
    int _reconnectInterval;
    bool _debug;

    // Сканирование
    bool _scanning;
    bool _scanComplete;
    unsigned long _scanStartTime;
    std::vector<NetworkInfo> _scanResults;

    // mDNS
    bool _mdnsActive;
    String _mdnsHostname;

    // Колбэки
    WiFiStatusCallback _statusCallback;
    NetworkFoundCallback _networkFoundCallback;
    WiFiMessageCallback _messageCallback;

    // Внутренние методы
    void _updateStatus(WiFiStatus newStatus);
    void _log(const String &message);
    void _log(const String &message, const String &value);
    String _encryptionTypeToString(uint8_t type);
    void _applyHostname();

    // Статические обработчики событий WiFi
    static void _onWiFiEvent(WiFiEvent_t event, arduino_event_info_t info);
    static WiFiManager *_instance;

    // Обработчики событий
    void _handleWiFiEvent(WiFiEvent_t event);
};

#endif