#include "WiFiManager.h"
#include <Preferences.h>

// Статический экземпляр для обработки событий
WiFiManager *WiFiManager::_instance = nullptr;

WiFiManager::WiFiManager()
    : _status(WiFiStatus::DISCONNECTED), _mode(WiFiMode::STATION), _timeout(20000), _reconnectInterval(5000), _debug(false), _scanning(false), _scanComplete(false), _mdnsActive(false), _statusCallback(nullptr), _networkFoundCallback(nullptr), _messageCallback(nullptr)
{
    _instance = this;
}

WiFiManager::~WiFiManager()
{
    _instance = nullptr;
}

// ===== ИНИЦИАЛИЗАЦИЯ =====

void WiFiManager::begin()
{
    _log("WiFiManager initialized");

    // Регистрация обработчика событий
    WiFi.onEvent(_onWiFiEvent);

    // Применение имени хоста
    _applyHostname();

    // Проверка сохраненных учетных данных
    String savedSSID, savedPass;
    if (loadCredentials(savedSSID, savedPass))
    {
        _log("Saved credentials found: " + savedSSID);
    }
}

// ===== ПОДКЛЮЧЕНИЕ =====

bool WiFiManager::connect(const String &ssid, const String &password, int timeout)
{
    if (ssid.isEmpty())
    {
        _log("ERROR: SSID is empty");
        return false;
    }

    _ssid = ssid;
    _password = password;
    _timeout = timeout > 0 ? timeout : _timeout;

    _log("Connecting to: " + ssid);
    _updateStatus(WiFiStatus::CONNECTING);

    // Переключаемся в клиентский режим
    if (_mode == WiFiMode::ACCESS_POINT)
    {
        setMode(WiFiMode::BOTH);
    }
    else
    {
        setMode(WiFiMode::STATION);
    }

    // Подключаемся
    WiFi.begin(ssid.c_str(), password.c_str());

    _connectStartTime = millis();

    // Ожидание подключения
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - _connectStartTime > _timeout)
        {
            _log("Connection timeout");
            _updateStatus(WiFiStatus::FAILED);
            return false;
        }
        delay(50);
    }

    _localIP = WiFi.localIP();
    _log("Connected! IP: " + _localIP.toString());
    _updateStatus(WiFiStatus::CONNECTED);

    // Сохраняем учетные данные
    saveCredentials(ssid, password);

    return true;
}

bool WiFiManager::connectSaved(int timeout)
{
    String savedSSID, savedPass;
    if (!loadCredentials(savedSSID, savedPass))
    {
        _log("No saved credentials found");
        return false;
    }

    return connect(savedSSID, savedPass, timeout);
}

void WiFiManager::disconnect()
{
    WiFi.disconnect();
    _updateStatus(WiFiStatus::DISCONNECTED);
    _log("Disconnected");
}

// ===== ТОЧКА ДОСТУПА =====

bool WiFiManager::startAccessPoint(const String &ssid, const String &password, int channel, bool hidden)
{
    _apSSID = ssid.isEmpty() ? "ESP32-AP" : ssid;
    _apPassword = password;

    _log("Starting AP: " + _apSSID);

    if (_mode == WiFiMode::STATION)
    {
        setMode(WiFiMode::BOTH);
    }
    else
    {
        setMode(WiFiMode::ACCESS_POINT);
    }

    bool success;
    if (_apPassword.length() >= 8)
    {
        success = WiFi.softAP(_apSSID.c_str(), _apPassword.c_str(), channel, hidden);
    }
    else if (_apPassword.isEmpty())
    {
        success = WiFi.softAP(_apSSID.c_str(), nullptr, channel, hidden);
    }
    else
    {
        _log("Warning: Password too short (min 8 chars). Using open network.");
        success = WiFi.softAP(_apSSID.c_str(), nullptr, channel, hidden);
    }

    if (success)
    {
        _apIP = WiFi.softAPIP();
        _log("AP started. IP: " + _apIP.toString());
        _updateStatus(WiFiStatus::AP_ACTIVE);
        return true;
    }
    else
    {
        _log("Failed to start AP");
        return false;
    }
}

void WiFiManager::stopAccessPoint()
{
    WiFi.softAPdisconnect(true);
    _log("AP stopped");
    if (_mode == WiFiMode::ACCESS_POINT)
    {
        setMode(WiFiMode::STATION);
    }
    _updateStatus(WiFiStatus::DISCONNECTED);
}

// ===== УПРАВЛЕНИЕ НАСТРОЙКАМИ =====

bool WiFiManager::saveCredentials(const String &ssid, const String &password)
{
    Preferences prefs;
    if (!prefs.begin("wifi", false))
    {
        _log("Failed to open preferences");
        return false;
    }

    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
    prefs.end();

    _log("Credentials saved");
    return true;
}

bool WiFiManager::loadCredentials(String &ssid, String &password)
{
    Preferences prefs;
    if (!prefs.begin("wifi", true))
    {
        return false;
    }

    ssid = prefs.getString("ssid", "");
    password = prefs.getString("pass", "");
    prefs.end();

    return !ssid.isEmpty();
}

void WiFiManager::clearCredentials()
{
    Preferences prefs;
    if (prefs.begin("wifi", false))
    {
        prefs.clear();
        prefs.end();
        _log("Credentials cleared");
    }
}

bool WiFiManager::hasSavedCredentials() const
{
    String ssid, pass;
    return const_cast<WiFiManager *>(this)->loadCredentials(ssid, pass) && !ssid.isEmpty();
}

// ===== РЕЖИМЫ =====

void WiFiManager::setMode(WiFiMode mode)
{
    _mode = mode;
    switch (mode)
    {
    case WiFiMode::STATION:
        WiFi.mode(WIFI_STA);
        break;
    case WiFiMode::ACCESS_POINT:
        WiFi.mode(WIFI_AP);
        break;
    case WiFiMode::BOTH:
        WiFi.mode(WIFI_AP_STA);
        break;
    }
    _log("Mode set to: " + String((int)mode));
}

// ===== ИНФОРМАЦИЯ О СОСТОЯНИИ =====

WiFiStatus WiFiManager::getStatus() const
{
    return _status;
}

IPAddress WiFiManager::getIP() const
{
    return WiFi.localIP();
}

IPAddress WiFiManager::getSubnet() const
{
    return WiFi.subnetMask();
}

IPAddress WiFiManager::getGateway() const
{
    return WiFi.gatewayIP();
}

IPAddress WiFiManager::getDNS() const
{
    return WiFi.dnsIP();
}

IPAddress WiFiManager::getAPIP() const
{
    return WiFi.softAPIP();
}

String WiFiManager::getMacAddress() const
{
    return WiFi.macAddress();
}

String WiFiManager::getAPMacAddress() const
{
    return WiFi.softAPmacAddress();
}

String WiFiManager::getSSID() const
{
    return WiFi.SSID();
}

int32_t WiFiManager::getRSSI() const
{
    return WiFi.RSSI();
}

uint8_t WiFiManager::getSignalQuality() const
{
    int32_t rssi = WiFi.RSSI();
    if (rssi <= -100)
        return 0;
    if (rssi >= -50)
        return 100;
    return (uint8_t)(2 * (rssi + 100));
}

unsigned long WiFiManager::getUptime() const
{
    return millis();
}

// ===== СКАНИРОВАНИЕ =====

std::vector<NetworkInfo> WiFiManager::scanNetworks(bool async)
{
    if (async)
    {
        startAsyncScan();
        return std::vector<NetworkInfo>();
    }

    _log("Scanning networks...");
    _scanResults.clear();

    int networksFound = WiFi.scanNetworks();

    if (networksFound == WIFI_SCAN_FAILED)
    {
        _log("Scan failed");
        return _scanResults;
    }

    for (int i = 0; i < networksFound; i++)
    {
        NetworkInfo info;
        info.ssid = WiFi.SSID(i);
        info.rssi = WiFi.RSSI(i);
        info.encryptionType = WiFi.encryptionType(i);
        info.bssid = WiFi.BSSIDstr(i);
        info.channel = WiFi.channel(i);
        _scanResults.push_back(info);

        if (_networkFoundCallback)
        {
            _networkFoundCallback(info);
        }
    }

    WiFi.scanDelete();
    _log("Found " + String(networksFound) + " networks");

    return _scanResults;
}

bool WiFiManager::startAsyncScan()
{
    if (_scanning)
        return false;

    _scanning = true;
    _scanComplete = false;
    _scanResults.clear();
    _scanStartTime = millis();

    WiFi.scanNetworks(true);
    _log("Async scan started");
    return true;
}

bool WiFiManager::isScanComplete() const
{
    return _scanComplete;
}

std::vector<NetworkInfo> WiFiManager::getScanResults()
{
    if (!_scanComplete)
    {
        int scanStatus = WiFi.scanComplete();

        if (scanStatus == WIFI_SCAN_RUNNING)
        {
            return _scanResults;
        }

        if (scanStatus == WIFI_SCAN_FAILED)
        {
            _scanning = false;
            _scanComplete = true;
            _log("Scan failed");
            return _scanResults;
        }

        if (scanStatus > 0)
        {
            for (int i = 0; i < scanStatus; i++)
            {
                NetworkInfo info;
                info.ssid = WiFi.SSID(i);
                info.rssi = WiFi.RSSI(i);
                info.encryptionType = WiFi.encryptionType(i);
                info.bssid = WiFi.BSSIDstr(i);
                info.channel = WiFi.channel(i);
                _scanResults.push_back(info);
            }

            WiFi.scanDelete();
            _scanning = false;
            _scanComplete = true;
            _log("Async scan complete. Found " + String(_scanResults.size()) + " networks");
        }
    }

    return _scanResults;
}

// ===== mDNS =====

bool WiFiManager::startMDNS(const String &hostname)
{
    if (_mdnsActive)
    {
        stopMDNS();
    }

    _mdnsHostname = hostname;

    if (MDNS.begin(hostname.c_str()))
    {
        _mdnsActive = true;
        _log("mDNS started: " + hostname + ".local");
        return true;
    }
    else
    {
        _log("mDNS failed to start");
        return false;
    }
}

void WiFiManager::stopMDNS()
{
    if (_mdnsActive)
    {
        MDNS.end();
        _mdnsActive = false;
        _log("mDNS stopped");
    }
}

void WiFiManager::addMDNSService(const String &service, const String &protocol, uint16_t port)
{
    if (_mdnsActive)
    {
        MDNS.addService(service.c_str(), protocol.c_str(), port);
        _log("mDNS service added: " + service + "." + protocol);
    }
}

// ===== КОЛБЭКИ =====

void WiFiManager::onStatusChange(WiFiStatusCallback callback)
{
    _statusCallback = callback;
}

void WiFiManager::onNetworkFound(NetworkFoundCallback callback)
{
    _networkFoundCallback = callback;
}

void WiFiManager::onMessage(WiFiMessageCallback callback)
{
    _messageCallback = callback;
}

// ===== АВТОМАТИЧЕСКОЕ ПЕРЕПОДКЛЮЧЕНИЕ =====

void WiFiManager::autoReconnect()
{
    if (_status == WiFiStatus::CONNECTED && WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    if (_status == WiFiStatus::CONNECTED && WiFi.status() != WL_CONNECTED)
    {
        _log("Connection lost");
        _updateStatus(WiFiStatus::DISCONNECTED);
    }

    if (_status == WiFiStatus::DISCONNECTED || _status == WiFiStatus::FAILED)
    {
        if (millis() - _lastReconnectAttempt > _reconnectInterval)
        {
            _lastReconnectAttempt = millis();
            String savedSSID, savedPass;
            if (loadCredentials(savedSSID, savedPass) && !savedSSID.isEmpty())
            {
                _log("Attempting to reconnect to: " + savedSSID);
                connect(savedSSID, savedPass, _timeout);
            }
            else
            {
                _log("No credentials to reconnect");
            }
        }
    }
}

// ===== СЛУЖЕБНЫЕ ФУНКЦИИ =====

void WiFiManager::setTimeout(int ms)
{
    _timeout = ms;
}

void WiFiManager::setReconnectInterval(int ms)
{
    _reconnectInterval = ms;
}

void WiFiManager::setDebug(bool enabled)
{
    _debug = enabled;
}

String WiFiManager::getInfoString() const
{
    String info;
    info += "=== WiFi Info ===\n";
    info += "Status: " + getStatusString() + "\n";
    info += "SSID: " + getSSID() + "\n";
    info += "IP: " + getIP().toString() + "\n";
    info += "Signal: " + String(getSignalQuality()) + "%\n";
    info += "MAC: " + getMacAddress() + "\n";
    return info;
}

String WiFiManager::getStatusString() const
{
    switch (_status)
    {
    case WiFiStatus::DISCONNECTED:
        return "Disconnected";
    case WiFiStatus::CONNECTING:
        return "Connecting";
    case WiFiStatus::CONNECTED:
        return "Connected";
    case WiFiStatus::FAILED:
        return "Failed";
    case WiFiStatus::AP_ACTIVE:
        return "AP Active";
    default:
        return "Unknown";
    }
}

// ===== ВНУТРЕННИЕ МЕТОДЫ =====

void WiFiManager::_updateStatus(WiFiStatus newStatus)
{
    if (_status != newStatus)
    {
        _status = newStatus;
        if (_statusCallback)
        {
            _statusCallback(newStatus);
        }
        _log("Status changed: " + getStatusString());
    }
}

void WiFiManager::_log(const String &message)
{
    if (_debug)
    {
        Serial.println("[WiFiManager] " + message);
    }
}

void WiFiManager::_log(const String &message, const String &value)
{
    if (_debug)
    {
        Serial.println("[WiFiManager] " + message + ": " + value);
    }
}

String WiFiManager::_encryptionTypeToString(uint8_t type)
{
    switch (type)
    {
    case WIFI_AUTH_OPEN:
        return "Open";
    case WIFI_AUTH_WEP:
        return "WEP";
    case WIFI_AUTH_WPA_PSK:
        return "WPA-PSK";
    case WIFI_AUTH_WPA2_PSK:
        return "WPA2-PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WPA/WPA2-PSK";
    default:
        return "Unknown";
    }
}

void WiFiManager::_applyHostname()
{
    String hostname = "esp32-" + String(ESP.getEfuseMac(), HEX);
    WiFi.setHostname(hostname.c_str());
}

void WiFiManager::_onWiFiEvent(WiFiEvent_t event, arduino_event_info_t info)
{
    if (_instance)
    {
        _instance->_handleWiFiEvent(event);
    }
}

void WiFiManager::_handleWiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        _log("Connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        _log("Disconnected from AP");
        if (_status == WiFiStatus::CONNECTED)
        {
            _updateStatus(WiFiStatus::DISCONNECTED);
        }
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        _localIP = WiFi.localIP();
        _log("Got IP: " + _localIP.toString());
        _updateStatus(WiFiStatus::CONNECTED);
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        _log("Lost IP");
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        _log("AP started");
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        _log("AP stopped");
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        _log("Client connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        _log("Client disconnected from AP");
        break;
    default:
        break;
    }
}