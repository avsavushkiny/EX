#include "OTAWebUpdater.h"
#include <SPIFFS.h>

// Конструктор
OTAWebUpdater::OTAWebUpdater()
    : _server(nullptr),
      _apSSID("EX-OTA"),
      _lastStatus("Ready")
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

// Инициализация как точка доступа
bool OTAWebUpdater::begin(const char *apSSID, const char *apPassword)
{
    if (apSSID)
    {
        _apSSID = apSSID;
    }

    // Настройка как точка доступа
    WiFi.mode(WIFI_AP);

    if (apPassword && strlen(apPassword) >= 8)
    {
        WiFi.softAP(apSSID, apPassword);
    }
    else
    {
        WiFi.softAP(apSSID);
        if (apPassword && strlen(apPassword) > 0 && strlen(apPassword) < 8)
        {
            Serial.println("Warning: Password too short (min 8 chars). Using open network.");
        }
    }

    _localIP = WiFi.softAPIP();

    Serial.println("Access Point started!");
    Serial.print("SSID: ");
    Serial.println(_apSSID);
    Serial.print("IP Address: ");
    Serial.println(_localIP);

    // Настройка mDNS
    if (!MDNS.begin(_apSSID.c_str()))
    {
        _lastStatus = "MDNS failed to start";
        Serial.println("MDNS failed to start!");
    }
    else
    {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("mDNS started: http://%s.local\n", _apSSID.c_str());
    }

    _lastStatus = "Ready - AP Mode";
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
    _server->on("/status", [this]()
                { handleStatus(); });
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

// Получение статуса
String OTAWebUpdater::getLastStatus() const
{
    return _lastStatus;
}

// Получение IP адреса
IPAddress OTAWebUpdater::getLocalIP() const
{
    return _localIP;
}

// HTML страница в стиле Microsoft
String OTAWebUpdater::getUpdatePageHTML()
{
    return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EX OTA Update</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;
            background: #f5f5f5;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        
        .container {
            background: white;
            max-width: 640px;
            width: 100%;
            border-radius: 8px;
            box-shadow: 0 2px 12px rgba(0, 0, 0, 0.08);
            padding: 40px;
        }
        
        .header {
            margin-bottom: 32px;
        }
        
        .header h1 {
            font-size: 28px;
            font-weight: 600;
            color: #1a1a1a;
            letter-spacing: -0.5px;
        }
        
        .header p {
            color: #6b6b6b;
            font-size: 14px;
            margin-top: 4px;
        }
        
        .divider {
            height: 1px;
            background: #e5e5e5;
            margin: 24px 0;
        }
        
        .info-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 16px;
            margin-bottom: 24px;
        }
        
        .info-item {
            background: #fafafa;
            padding: 12px 16px;
            border-radius: 4px;
            border: 1px solid #e5e5e5;
        }
        
        .info-item .label {
            font-size: 11px;
            text-transform: uppercase;
            color: #6b6b6b;
            font-weight: 600;
            letter-spacing: 0.5px;
            display: block;
            margin-bottom: 4px;
        }
        
        .info-item .value {
            font-size: 14px;
            color: #1a1a1a;
            font-weight: 500;
            word-break: break-all;
        }
        
        .info-item .value.mono {
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px;
        }
        
        .upload-section {
            margin: 24px 0;
        }
        
        .upload-box {
            border: 2px dashed #e5e5e5;
            border-radius: 4px;
            padding: 32px;
            text-align: center;
            transition: border-color 0.2s ease;
            cursor: pointer;
            background: #fafafa;
        }
        
        .upload-box:hover {
            border-color: #0078d4;
        }
        
        .upload-box.dragover {
            border-color: #0078d4;
            background: #f0f7ff;
        }
        
        .upload-box input[type="file"] {
            display: none;
        }
        
        .upload-icon {
            font-size: 32px;
            display: block;
            margin-bottom: 12px;
        }
        
        .upload-text {
            color: #1a1a1a;
            font-weight: 500;
            font-size: 16px;
        }
        
        .upload-hint {
            color: #6b6b6b;
            font-size: 13px;
            margin-top: 4px;
        }
        
        .file-name {
            color: #0078d4;
            font-weight: 500;
            margin-top: 8px;
            font-size: 14px;
            display: none;
        }
        
        .actions {
            display: flex;
            gap: 12px;
            margin-top: 24px;
        }
        
        .btn {
            padding: 10px 24px;
            border: none;
            border-radius: 4px;
            font-size: 14px;
            font-weight: 500;
            font-family: inherit;
            cursor: pointer;
            transition: all 0.15s ease;
            display: inline-flex;
            align-items: center;
            gap: 8px;
        }
        
        .btn-primary {
            background: #0078d4;
            color: white;
        }
        
        .btn-primary:hover:not(:disabled) {
            background: #0065b8;
        }
        
        .btn-primary:disabled {
            opacity: 0.5;
            cursor: not-allowed;
        }
        
        .btn-secondary {
            background: transparent;
            color: #6b6b6b;
            border: 1px solid #d0d0d0;
        }
        
        .btn-secondary:hover {
            background: #f0f0f0;
        }
        
        .btn-danger {
            background: #d13438;
            color: white;
        }
        
        .btn-danger:hover {
            background: #b12a2e;
        }
        
        .status {
            margin-top: 20px;
            padding: 12px 16px;
            border-radius: 4px;
            display: none;
            font-size: 14px;
        }
        
        .status.show {
            display: block;
        }
        
        .status-success {
            background: #dff6dd;
            color: #107c10;
            border: 1px solid #b7e0b4;
        }
        
        .status-error {
            background: #fde7e9;
            color: #a4262c;
            border: 1px solid #f9c9cc;
        }
        
        .status-info {
            background: #e7f0f9;
            color: #004578;
            border: 1px solid #c7d9ed;
        }
        
        .progress-container {
            margin-top: 16px;
            display: none;
        }
        
        .progress-container.show {
            display: block;
        }
        
        .progress-bar {
            width: 100%;
            height: 4px;
            background: #e5e5e5;
            border-radius: 2px;
            overflow: hidden;
        }
        
        .progress-bar .fill {
            height: 100%;
            background: #0078d4;
            width: 0%;
            transition: width 0.2s ease;
        }
        
        .progress-label {
            display: flex;
            justify-content: space-between;
            font-size: 13px;
            color: #6b6b6b;
            margin-top: 4px;
        }
        
        @media (max-width: 480px) {
            .container {
                padding: 24px;
            }
            
            .info-grid {
                grid-template-columns: 1fr;
            }
            
            .actions {
                flex-direction: column;
            }
            
            .btn {
                justify-content: center;
            }
            
            .header h1 {
                font-size: 24px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>ESP32 Update</h1>
            <p>Upload firmware to update your device</p>
        </div>
        
        <div class="info-grid">
            <div class="info-item">
                <span class="label">Device</span>
                <span class="value">)rawliteral" +
           _apSSID + R"rawliteral(</span>
            </div>
            <div class="info-item">
                <span class="label">IP Address</span>
                <span class="value mono">)rawliteral" +
           _localIP.toString() + R"rawliteral(</span>
            </div>
            <div class="info-item">
                <span class="label">Firmware</span>
                <span class="value mono" id="firmwareVersion">)rawliteral" +
           ESP.getSketchMD5().substring(0, 16) + R"rawliteral(</span>
            </div>
            <div class="info-item">
                <span class="label">Memory</span>
                <span class="value mono">)rawliteral" +
           String(ESP.getFreeHeap() / 1024) + R"rawliteral( KB free</span>
            </div>
        </div>
        
        <div class="divider"></div>
        
        <form id="uploadForm" method="POST" action="/update" enctype="multipart/form-data">
            <div class="upload-section">
                <div class="upload-box" id="uploadBox">
                    <span class="upload-icon">📁</span>
                    <div class="upload-text">Choose or drag a .bin file</div>
                    <div class="upload-hint">Firmware file up to 4MB</div>
                    <div class="file-name" id="fileName"></div>
                    <input type="file" id="fileInput" name="firmware" accept=".bin">
                </div>
            </div>
            
            <div class="actions">
                <button type="submit" class="btn btn-primary" id="uploadBtn" disabled>
                    <span>⬆</span> Upload
                </button>
                <button type="button" class="btn btn-secondary" onclick="location.reload()">
                    ⟳ Refresh
                </button>
                <button type="button" class="btn btn-danger" onclick="resetDevice()">
                    ↺ Reset
                </button>
            </div>
        </form>
        
        <div class="status" id="statusMessage"></div>
        
        <div class="progress-container" id="progressContainer">
            <div class="progress-bar">
                <div class="fill" id="progressFill"></div>
            </div>
            <div class="progress-label">
                <span id="progressText">Uploading...</span>
                <span id="progressPercent">0%</span>
            </div>
        </div>
    </div>

    <script>
        const fileInput = document.getElementById('fileInput');
        const uploadBox = document.getElementById('uploadBox');
        const fileName = document.getElementById('fileName');
        const uploadBtn = document.getElementById('uploadBtn');
        const statusMsg = document.getElementById('statusMessage');
        const progressContainer = document.getElementById('progressContainer');
        const progressFill = document.getElementById('progressFill');
        const progressText = document.getElementById('progressText');
        const progressPercent = document.getElementById('progressPercent');
        const form = document.getElementById('uploadForm');

        // File selection
        fileInput.addEventListener('change', function() {
            if (this.files && this.files[0]) {
                const file = this.files[0];
                fileName.textContent = '📄 ' + file.name + ' (' + (file.size / 1024).toFixed(1) + ' KB)';
                fileName.style.display = 'block';
                uploadBtn.disabled = false;
                hideStatus();
            }
        });

        // Drag and drop
        uploadBox.addEventListener('dragover', function(e) {
            e.preventDefault();
            this.classList.add('dragover');
        });

        uploadBox.addEventListener('dragleave', function(e) {
            e.preventDefault();
            this.classList.remove('dragover');
        });

        uploadBox.addEventListener('drop', function(e) {
            e.preventDefault();
            this.classList.remove('dragover');
            if (e.dataTransfer.files && e.dataTransfer.files[0]) {
                fileInput.files = e.dataTransfer.files;
                fileInput.dispatchEvent(new Event('change'));
            }
        });

        uploadBox.addEventListener('click', function() {
            fileInput.click();
        });

        // Form submit
        form.addEventListener('submit', function(e) {
            e.preventDefault();
            
            if (!fileInput.files || !fileInput.files[0]) {
                showStatus('Please select a firmware file', 'error');
                return;
            }

            const file = fileInput.files[0];
            if (!file.name.toLowerCase().endsWith('.bin')) {
                showStatus('Please select a .bin file', 'error');
                return;
            }

            uploadBtn.disabled = true;
            showStatus('Uploading firmware...', 'info');
            showProgress(0, 'Starting upload...');

            const xhr = new XMLHttpRequest();
            const formData = new FormData(form);

            xhr.upload.addEventListener('progress', function(e) {
                if (e.lengthComputable) {
                    const percent = Math.round((e.loaded / e.total) * 100);
                    showProgress(percent);
                }
            });

            xhr.addEventListener('load', function() {
                if (xhr.status === 200) {
                    const response = xhr.responseText;
                    if (response.includes('Success')) {
                        showStatus('✅ Update successful! Device will restart.', 'success');
                        showProgress(100, 'Update complete!');
                        setTimeout(() => location.reload(), 3000);
                    } else {
                        showStatus('❌ ' + response, 'error');
                        uploadBtn.disabled = false;
                    }
                } else {
                    showStatus('❌ Upload failed: ' + xhr.statusText, 'error');
                    uploadBtn.disabled = false;
                }
            });

            xhr.addEventListener('error', function() {
                showStatus('❌ Network error', 'error');
                uploadBtn.disabled = false;
                hideProgress();
            });

            xhr.open('POST', '/update');
            xhr.send(formData);
        });

        function showStatus(message, type) {
            statusMsg.className = 'status show status-' + type;
            statusMsg.textContent = message;
        }

        function hideStatus() {
            statusMsg.className = 'status';
            statusMsg.textContent = '';
        }

        function showProgress(percent, text) {
            progressContainer.classList.add('show');
            progressFill.style.width = percent + '%';
            progressPercent.textContent = percent + '%';
            if (text) progressText.textContent = text;
        }

        function hideProgress() {
            progressContainer.classList.remove('show');
            progressFill.style.width = '0%';
            progressPercent.textContent = '0%';
        }

        function resetDevice() {
            if (confirm('Reset the device?')) {
                showStatus('Resetting...', 'info');
                fetch('/update', { method: 'POST' })
                    .then(() => {
                        showStatus('Device reset', 'success');
                        setTimeout(() => location.reload(), 2000);
                    })
                    .catch(() => {
                        showStatus('Reset failed', 'error');
                    });
            }
        }

        // Refresh status
        setInterval(function() {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    if (data.version) {
                        document.getElementById('firmwareVersion').textContent = data.version.substring(0, 16);
                    }
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
    _server->send(200, "text/html", getUpdatePageHTML());
}

// Обработчик обновления (POST)
void OTAWebUpdater::handleUpdate()
{
    _lastStatus = "Update initiated";
    _server->send(200, "text/plain", "Update Success");
    delay(500);
    ESP.restart();
}

// Обработчик загрузки прошивки
void OTAWebUpdater::handleFirmwareUpload()
{
    HTTPUpload &upload = _server->upload();

    if (upload.status == UPLOAD_FILE_START)
    {
        Serial.printf("Update start: %s\n", upload.filename.c_str());
        _lastStatus = "Uploading: " + String(upload.filename.c_str());

        if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        {
            Update.printError(Serial);
            _lastStatus = "Update error: " + String(Update.getError());
            return;
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            Update.printError(Serial);
            _lastStatus = "Write error";
            return;
        }
        Serial.printf("Progress: %d bytes\n", upload.totalSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
            Serial.printf("Update success: %d bytes\n", upload.totalSize);
            _lastStatus = "Update successful!";
            _server->send(200, "text/plain", "Update Success");
            delay(500);
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

// Обработчик статуса
void OTAWebUpdater::handleStatus()
{
    String json = "{";
    json += "\"status\":\"" + _lastStatus + "\",";
    json += "\"version\":\"" + ESP.getSketchMD5() + "\",";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += "}";
    _server->send(200, "application/json", json);
}

// Обработчик 404
void OTAWebUpdater::handleNotFound()
{
    _server->send(404, "text/html",
                  "<html><body><h1>404</h1><p>Page not found</p></body></html>");
}

// Вспомогательные функции
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