#include "OTAWebUpdater.h"
#include <SPIFFS.h>

// Конструктор
OTAWebUpdater::OTAWebUpdater()
    : _server(nullptr),
      _hostname("ESP32-OTA"),
      _lastStatus("Ready"),
      _password(""),
      _passwordProtected(false)
{
}

// Деструктор
OTAWebUpdater::~OTAWebUpdater()
{
    if (_server)
    {
        _server->stop();
        delete _server;
    }
}

// Инициализация
bool OTAWebUpdater::begin(const char *ssid, const char *password, const char *hostname)
{
    if (hostname)
    {
        _hostname = hostname;
    }

    // Подключение к WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED)
    {
        _lastStatus = "WiFi connection failed";
        Serial.println("WiFi connection failed!");
        return false;
    }

    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Настройка mDNS
    if (!MDNS.begin(_hostname.c_str()))
    {
        _lastStatus = "MDNS failed to start";
        Serial.println("MDNS failed to start!");
        return false;
    }

    MDNS.addService("http", "tcp", 80);
    Serial.printf("mDNS responder started: http://%s.local\n", _hostname.c_str());

    _lastStatus = "Ready";
    return true;
}

// Запуск веб-сервера
void OTAWebUpdater::startWebServer(int port)
{
    if (_server)
    {
        delete _server;
    }

    _server = new WebServer(port);

    // Настройка маршрутов
    _server->on("/", [this]()
                { handleRoot(); });
    _server->on("/update", HTTP_POST, [this]()
                { handleUpdate(); }, [this]()
                { handleFirmwareUpload(); });
    _server->on("/reset", HTTP_POST, [this]()
                { handleReset(); });
    _server->on("/status", [this]()
                { _server->send(200, "application/json",
                                "{\"status\":\"" + _lastStatus + "\",\"version\":\"" + ESP.getSketchMD5() + "\"}"); });
    _server->onNotFound([this]()
                        { handleNotFound(); });

    _server->begin();
    Serial.println("HTTP server started");

    _lastStatus = "Web server running";
}

// Обработка клиентов
void OTAWebUpdater::handleClient()
{
    if (_server)
    {
        _server->handleClient();
    }
}

// Проверка обновлений (заглушка для будущей реализации)
bool OTAWebUpdater::checkForUpdate()
{
    // Здесь можно реализовать проверку на сервере
    return false;
}

// Получение статуса
String OTAWebUpdater::getLastStatus() const
{
    return _lastStatus;
}

// Установка пароля
void OTAWebUpdater::setPassword(const char *password)
{
    if (password && strlen(password) > 0)
    {
        _password = password;
        _passwordProtected = true;
    }
    else
    {
        _passwordProtected = false;
    }
}

// HTML страница обновления
String OTAWebUpdater::getUpdatePageHTML()
{
    return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EX OTA Update</title>
    <style>
        * { box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: rgba(255,255,255,0.95);
            border-radius: 16px;
            padding: 30px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
        }
        h1 {
            color: #333;
            border-bottom: 3px solid #667eea;
            padding-bottom: 15px;
            margin-bottom: 25px;
        }
        .info {
            background: #f8f9fa;
            border-radius: 8px;
            padding: 15px;
            margin: 20px 0;
        }
        .info-item {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            border-bottom: 1px solid #e9ecef;
        }
        .info-item:last-child { border-bottom: none; }
        .info-label { font-weight: bold; color: #555; }
        .info-value { color: #333; font-family: monospace; }
        .upload-area {
            border: 3px dashed #667eea;
            border-radius: 12px;
            padding: 30px;
            text-align: center;
            margin: 20px 0;
            transition: all 0.3s ease;
            background: #f8f9ff;
        }
        .upload-area:hover { border-color: #764ba2; background: #f0f0ff; }
        .upload-area input[type="file"] { display: none; }
        .upload-label {
            display: inline-block;
            padding: 15px 30px;
            background: #667eea;
            color: white;
            border-radius: 8px;
            cursor: pointer;
            transition: background 0.3s ease;
            font-weight: bold;
        }
        .upload-label:hover { background: #764ba2; }
        .btn {
            display: inline-block;
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s ease;
            margin: 5px;
        }
        .btn-primary { background: #667eea; color: white; }
        .btn-primary:hover { background: #764ba2; transform: translateY(-2px); }
        .btn-danger { background: #dc3545; color: white; }
        .btn-danger:hover { background: #c82333; transform: translateY(-2px); }
        .btn-success { background: #28a745; color: white; }
        .btn-success:hover { background: #218838; transform: translateY(-2px); }
        .status {
            margin: 20px 0;
            padding: 15px;
            border-radius: 8px;
            display: none;
        }
        .status.show { display: block; }
        .status-success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .status-error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        .status-info { background: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
        .progress {
            display: none;
            width: 100%;
            height: 30px;
            background: #e9ecef;
            border-radius: 15px;
            overflow: hidden;
            margin: 15px 0;
        }
        .progress.show { display: block; }
        .progress-bar {
            height: 100%;
            background: linear-gradient(90deg, #667eea, #764ba2);
            width: 0%;
            transition: width 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: bold;
        }
        @media (max-width: 600px) {
            .container { padding: 15px; }
            .info-item { flex-direction: column; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>EX OTA Update</h1>
        
        <div class="info">
            <div class="info-item">
                <span class="info-label">Device:</span>
                <span class="info-value">)rawliteral" +
           _hostname + R"rawliteral(</span>
            </div>
            <div class="info-item">
                <span class="info-label">IP Address:</span>
                <span class="info-value">)rawliteral" +
           WiFi.localIP().toString() + R"rawliteral(</span>
            </div>
            <div class="info-item">
                <span class="info-label">Current Firmware:</span>
                <span class="info-value" id="currentFirmware">)rawliteral" +
           ESP.getSketchMD5() + R"rawliteral(</span>
            </div>
            <div class="info-item">
                <span class="info-label">Free Heap:</span>
                <span class="info-value">)rawliteral" +
           String(ESP.getFreeHeap()) + R"rawliteral( bytes</span>
            </div>
        </div>

        <div class="upload-area">
            <form id="uploadForm" method="POST" action="/update" enctype="multipart/form-data">
                <input type="file" id="firmware" name="firmware" accept=".bin">
                <label for="firmware" class="upload-label">📁 Choose Firmware File</label>
                <br><br>
                <button type="submit" class="btn btn-primary">⬆ Upload & Update</button>
            </form>
            <p style="margin-top: 15px; color: #666; font-size: 14px;">
                Select a .bin firmware file and click upload
            </p>
        </div>

        <div id="statusMessage" class="status"></div>
        <div id="progressContainer" class="progress">
            <div id="progressBar" class="progress-bar" style="width: 0%;">0%</div>
        </div>

        <div style="text-align: center; margin-top: 25px;">
            <button onclick="window.location.reload()" class="btn btn-success">🔄 Refresh Status</button>
            <button onclick="resetDevice()" class="btn btn-danger">🔴 Reset Device</button>
        </div>
    </div>

    <script>
        const statusMessage = document.getElementById('statusMessage');
        const progressContainer = document.getElementById('progressContainer');
        const progressBar = document.getElementById('progressBar');
        const uploadForm = document.getElementById('uploadForm');

        function showStatus(message, type = 'info') {
            statusMessage.className = 'status show status-' + type;
            statusMessage.textContent = message;
            statusMessage.style.display = 'block';
        }

        function updateProgress(value) {
            progressContainer.className = 'progress show';
            progressBar.style.width = value + '%';
            progressBar.textContent = Math.round(value) + '%';
        }

        uploadForm.addEventListener('submit', function(e) {
            const fileInput = document.getElementById('firmware');
            if (!fileInput.files || !fileInput.files[0]) {
                e.preventDefault();
                showStatus('Please select a firmware file first', 'error');
                return;
            }

            const file = fileInput.files[0];
            if (!file.name.endsWith('.bin')) {
                e.preventDefault();
                showStatus('Please select a .bin firmware file', 'error');
                return;
            }

            showStatus('Uploading firmware...', 'info');
            updateProgress(0);

            const xhr = new XMLHttpRequest();
            const formData = new FormData(uploadForm);

            xhr.upload.addEventListener('progress', function(e) {
                if (e.lengthComputable) {
                    const percent = (e.loaded / e.total) * 100;
                    updateProgress(percent);
                }
            });

            xhr.addEventListener('load', function() {
                if (xhr.status === 200) {
                    const response = xhr.responseText;
                    if (response.includes('Update Successful')) {
                        showStatus('✅ Update successful! Device will restart in 5 seconds...', 'success');
                        setTimeout(() => {
                            window.location.reload();
                        }, 5000);
                    } else {
                        showStatus('❌ ' + response, 'error');
                    }
                } else {
                    showStatus('❌ Upload failed: ' + xhr.statusText, 'error');
                }
                updateProgress(100);
            });

            xhr.addEventListener('error', function() {
                showStatus('❌ Network error during upload', 'error');
                updateProgress(0);
            });

            xhr.open('POST', '/update');
            xhr.send(formData);
            e.preventDefault();
        });

        function resetDevice() {
            if (confirm('Are you sure you want to reset the device?')) {
                showStatus('Resetting device...', 'info');
                fetch('/reset', { method: 'POST' })
                    .then(response => {
                        showStatus('Device is resetting...', 'success');
                        setTimeout(() => window.location.reload(), 3000);
                    })
                    .catch(error => {
                        showStatus('Reset failed: ' + error, 'error');
                    });
            }
        }

        // Poll status every 5 seconds
        setInterval(function() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('currentFirmware').textContent = data.version || 'Unknown';
                })
                .catch(() => {});
        }, 5000);
    </script>
</body>
</html>
)rawliteral";
}

// Обработчик корневой страницы
void OTAWebUpdater::handleRoot()
{
    if (_passwordProtected)
    {
        if (!_server->authenticate("admin", _password.c_str()))
        {
            return _server->requestAuthentication();
        }
    }
    _server->send(200, "text/html", getUpdatePageHTML());
}

// Обработчик обновления (POST)
void OTAWebUpdater::handleUpdate()
{
    if (_passwordProtected)
    {
        if (!_server->authenticate("admin", _password.c_str()))
        {
            return _server->requestAuthentication();
        }
    }
    _lastStatus = "Update initiated";
    _server->send(200, "text/html", "Update successful! Device will restart.");
    ESP.restart();
}

// Обработчик загрузки прошивки
void OTAWebUpdater::handleFirmwareUpload()
{
    if (_passwordProtected)
    {
        if (!_server->authenticate("admin", _password.c_str()))
        {
            return _server->requestAuthentication();
        }
    }

    HTTPUpload &upload = _server->upload();

    if (upload.status == UPLOAD_FILE_START)
    {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        _lastStatus = "Uploading firmware...";

        if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        {
            Update.printError(Serial);
            _lastStatus = "Update failed: " + String(Update.getError());
            return;
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            Update.printError(Serial);
            _lastStatus = "Write error: " + String(Update.getError());
            return;
        }
        Serial.printf("Progress: %d\n", upload.totalSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
            Serial.printf("Update Success: %d bytes\n", upload.totalSize);
            _lastStatus = "Firmware update successful!";
            _server->send(200, "text/plain", "Update successful! Device will restart.");
            delay(1000);
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
            _lastStatus = "Update failed: " + String(Update.getError());
            _server->send(500, "text/plain", "Update failed: " + String(Update.getError()));
        }
    }
}

// Обработчик сброса
void OTAWebUpdater::handleReset()
{
    if (_passwordProtected)
    {
        if (!_server->authenticate("admin", _password.c_str()))
        {
            return _server->requestAuthentication();
        }
    }
    _server->send(200, "text/plain", "Resetting device...");
    delay(1000);
    ESP.restart();
}

// Обработчик 404
void OTAWebUpdater::handleNotFound()
{
    _server->send(404, "text/html",
                  "<html><body><h1>404 Not Found</h1><p>The requested page was not found.</p></body></html>");
}

// Вспомогательные функции
String OTAWebUpdater::getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".bin"))
        return "application/octet-stream";
    return "text/plain";
}

String OTAWebUpdater::getBytes(uint64_t bytes)
{
    const char *suffixes[] = {"B", "KB", "MB", "GB"};
    int i = 0;
    double size = bytes;
    while (size >= 1024 && i < 3)
    {
        size /= 1024;
        i++;
    }
    return String(size, 1) + " " + suffixes[i];
}