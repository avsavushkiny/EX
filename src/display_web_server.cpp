#include "display_web_server.h"
#include <SPIFFS.h>

// Конструктор
DisplayWebServer::DisplayWebServer()
    : _server(nullptr),
      _apSSID("Display-Server"),
      _lastStatus("Ready"),
      _displayBuffer(nullptr),
      _width(256),
      _height(160),
      _hasBuffer(false)
{
}

// Деструктор
DisplayWebServer::~DisplayWebServer()
{
    if (_server)
    {
        _server->stop();
        delete _server;
    }
}

// Инициализация как точка доступа
bool DisplayWebServer::begin(const char *apSSID, const char *apPassword)
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
    Serial.println("Display Web Server started!");
    Serial.print("Connect to: http://");
    Serial.println(_localIP);
    
    return true;
}

// Запуск веб-сервера
void DisplayWebServer::startWebServer(int port)
{
    if (_server)
    {
        delete _server;
    }

    _server = new WebServer(port);

    // Настройка маршрутов
    _server->on("/", [this]()
                { handleRoot(); });
    _server->on("/image.bmp", [this]()
                { handleDisplayImage(); });
    _server->on("/status", [this]()
                { handleStatus(); });
    _server->onNotFound([this]()
                        { handleNotFound(); });

    _server->begin();
    _lastStatus = "Web server running";
    Serial.println("HTTP server started");
}

// Обработка клиентов
void DisplayWebServer::handleClient()
{
    if (_server)
    {
        _server->handleClient();
    }
}

// Установка буфера дисплея
void DisplayWebServer::setDisplayBuffer(uint8_t *buffer, int width, int height)
{
    _displayBuffer = buffer;
    _width = width;
    _height = height;
    _hasBuffer = (buffer != nullptr);
}

// Получение статуса
String DisplayWebServer::getLastStatus() const
{
    return _lastStatus;
}

// Получение IP адреса
IPAddress DisplayWebServer::getLocalIP() const
{
    return _localIP;
}

// Генерация HTML страницы
String DisplayWebServer::getDisplayPageHTML()
{
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Display Server</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}
        body{background:#0e0e14;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:16px;font-family:'JetBrains Mono','Consolas',monospace;color:#c8d0dc;line-height:1.5}
        .console{max-width:800px;width:100%;background:#14161e;border-radius:12px;border:1px solid #2a2d3a;box-shadow:0 12px 48px rgba(0,0,0,.7);padding:20px 24px 24px}
        @media(max-width:600px){.console{padding:14px 14px 18px}}
        .terminal-bar{display:flex;align-items:center;gap:10px;margin-bottom:16px;padding-bottom:10px;border-bottom:1px solid #252a36}
        .dot-group{display:flex;gap:6px}
        .dot{width:10px;height:10px;border-radius:50%}
        .dot.red{background:#ff5f6d}
        .dot.yellow{background:#ffc857}
        .dot.green{background:#2bcf7a}
        .terminal-title{font-size:13px;color:#6b728a;font-weight:400;user-select:none}
        .terminal-title span{color:#3b82f6}
        .header{margin-bottom:16px}
        .header h1{font-size:18px;font-weight:500;color:#e8edf5}
        .header p{font-size:12px;color:#6e768f;margin-top:1px}
        .header p .prompt{color:#3b82f6}
        .divider{height:1px;background:#232835;margin:14px 0 16px}
        .info-grid{display:grid;grid-template-columns:1fr 1fr;gap:6px;margin-bottom:2px}
        @media(max-width:420px){.info-grid{grid-template-columns:1fr}}
        .info-item{background:#0f111a;border:1px solid #222734;border-radius:6px;padding:8px 12px}
        .info-item .label{font-size:8px;text-transform:uppercase;letter-spacing:.5px;color:#5c6480;display:block;font-weight:500}
        .info-item .value{font-size:13px;color:#d4dcec;word-break:break-word;margin-top:1px}
        .info-item .value.mono{font-size:12px;color:#a8b5d1}
        .display-container{background:#0a0c12;border-radius:8px;border:2px solid #1a1f2a;padding:12px;margin:8px 0 2px;text-align:center}
        .display-container img{max-width:100%;height:auto;border-radius:4px;image-rendering:pixelated;background:#000}
        .display-container .no-image{color:#4a526a;padding:30px 20px;font-size:13px}
        .display-container .no-image .cmd{color:#3b82f6;background:#1a2030;padding:0 8px;border-radius:3px;border:1px solid #2a3145}
        .actions{display:flex;flex-wrap:wrap;gap:6px;margin-top:14px}
        .btn{padding:6px 16px;border:1px solid #2a3145;border-radius:30px;font-size:11px;font-weight:450;font-family:inherit;cursor:pointer;transition:background .15s,border-color .15s;display:inline-flex;align-items:center;gap:4px;background:#11141f;color:#b0bbd4}
        .btn:active{transform:scale(.97)}
        .btn-primary{background:#1a2a44;border-color:#3b82f6;color:#b6ceff}
        .btn-primary:hover{background:#1f3155;border-color:#5b94f7}
        .btn-secondary{background:0 0;border-color:#232835;color:#6b748f}
        .btn-secondary:hover{background:#181d2b;border-color:#3a4058}
        @media(max-width:420px){.actions{flex-direction:column}.btn{justify-content:center;padding:8px 16px}}
        .status{margin-top:14px;padding:8px 14px;border-radius:5px;display:none;font-size:11px;border-left:3px solid transparent;background:#0f111a}
        .status.show{display:block}
        .status-info{border-left-color:#3b82f6;color:#9bb8f0;background:#111a2a}
        .status-success{border-left-color:#2bcf7a;color:#8cd4b0;background:#0f1d16}
        .status-error{border-left-color:#ff5f6d;color:#f0a0a8;background:#1e1216}
        ::-webkit-scrollbar{width:4px;background:#0e0e14}
        ::-webkit-scrollbar-thumb{background:#2a3145;border-radius:8px}
        ::selection{background:rgba(59,130,246,.2);color:#e8edf5}
        .auto-refresh{display:flex;align-items:center;gap:8px;font-size:11px;color:#6e768f;margin-top:8px;justify-content:center}
        .auto-refresh input[type="checkbox"]{accent-color:#3b82f6;width:16px;height:16px;cursor:pointer}
    </style>
</head>
<body>
<div class="console">
    <div class="terminal-bar">
        <div class="dot-group">
            <span class="dot red"></span>
            <span class="dot yellow"></span>
            <span class="dot green"></span>
        </div>
        <div class="terminal-title"><span>DISPLAY</span></div>
    </div>
    <div class="header">
        <h1>⌨ Display Monitor</h1>
        <p><span class="prompt">$</span> live display · <span style="color:#6e768f;">streaming</span></p>
    </div>
    <div class="divider"></div>
    <div class="info-grid">
        <div class="info-item"><span class="label">◆ device</span><span class="value">)rawliteral" + _apSSID + R"rawliteral(</span></div>
        <div class="info-item"><span class="label">◆ ip</span><span class="value mono">)rawliteral" + _localIP.toString() + R"rawliteral(</span></div>
        <div class="info-item"><span class="label">◆ resolution</span><span class="value mono">)rawliteral" + String(_width) + "×" + String(_height) + R"rawliteral(</span></div>
        <div class="info-item"><span class="label">◆ memory</span><span class="value mono">)rawliteral" + String(ESP.getFreeHeap()/1024) + R"rawliteral( <span style="color:#545d7a;">KB</span></span></div>
    </div>
    <div class="divider"></div>
    <div class="display-container">
)rawliteral";

    if (_hasBuffer)
    {
        html += R"rawliteral(
        <img id="displayImage" src="/image.bmp?t=)rawliteral" + String(millis()) + R"rawliteral(" alt="Display" />
)rawliteral";
    }
    else
    {
        html += R"rawliteral(
        <div class="no-image">⦿ <span class="cmd">$ no buffer</span> — display not initialized</div>
)rawliteral";
    }

    html += R"rawliteral(
    </div>
    <div class="auto-refresh">
        <input type="checkbox" id="autoRefresh" checked />
        <label for="autoRefresh">auto-refresh</label>
        <span style="margin-left:auto;color:#4a526a;" id="refreshTime">1s</span>
    </div>
    <div class="actions">
        <button class="btn btn-primary" onclick="refreshImage()">⟳ Refresh</button>
        <button class="btn btn-secondary" onclick="location.reload()">↺ Reload</button>
        <a href="/image.bmp" download="display.bmp" class="btn btn-secondary">⬇ Download</a>
    </div>
    <div class="status" id="status"></div>
</div>
<script>
(function(){
const img = document.getElementById('displayImage');
const status = document.getElementById('status');
const autoRefresh = document.getElementById('autoRefresh');
const refreshTime = document.getElementById('refreshTime');

function show(m,t){status.className='status show status-'+t;status.textContent=m;setTimeout(()=>{status.className='status';status.textContent=''},3000)}
function refreshImage(){if(img){const ts=Date.now();img.src='/image.bmp?t='+ts;img.onload=()=>show('✓ updated','success');img.onerror=()=>show('✗ load failed','error')}}

let refreshInterval = null;
function toggleRefresh(){if(autoRefresh.checked){refreshInterval=setInterval(refreshImage,1000);refreshTime.textContent='1s'}else{if(refreshInterval){clearInterval(refreshInterval);refreshInterval=null;refreshTime.textContent='stopped'}}}
autoRefresh.addEventListener('change',toggleRefresh);

// Click on image to refresh
if(img){img.addEventListener('click',refreshImage)}

// Initial state
if(autoRefresh.checked){toggleRefresh()}

// Status polling
setInterval(function(){
    fetch('/status').then(r=>r.json()).then(d=>{
        if(d.buffer){document.querySelector('.display-container .no-image')?.remove()}
    }).catch(()=>{})
},5000)
})();
</script>
</body>
</html>
)rawliteral";

    return html;
}

// Конвертация буфера в BMP (исправленная версия)
std::vector<uint8_t> DisplayWebServer::convertToBMP()
{
    std::vector<uint8_t> result;
    
    if (!_hasBuffer || !_displayBuffer)
    {
        return result;
    }
    
    // Параметры BMP
    int bpp = 1; // 8 бит на пиксель (4 уровня серого)
    int rowSize = ((_width * bpp + 3) / 4) * 4; // Выравнивание по 4 байта
    int imageSize = rowSize * _height;
    int paletteSize = 256 * 4; // 256 цветов * 4 байта
    int fileSize = 14 + 40 + paletteSize + imageSize;
    
    result.resize(fileSize);
    uint8_t *data = result.data();
    int offset = 0;
    
    // BITMAPFILEHEADER
    data[offset++] = 'B';
    data[offset++] = 'M';
    *(uint32_t*)(data + offset) = fileSize;
    offset += 4;
    *(uint32_t*)(data + offset) = 0;
    offset += 4;
    *(uint32_t*)(data + offset) = 14 + 40 + paletteSize;
    offset += 4;
    
    // BITMAPINFOHEADER
    *(uint32_t*)(data + offset) = 40;
    offset += 4;
    *(int32_t*)(data + offset) = _width;
    offset += 4;
    *(int32_t*)(data + offset) = -_height; // Сверху вниз
    offset += 4;
    *(uint16_t*)(data + offset) = 1;
    offset += 2;
    *(uint16_t*)(data + offset) = 8;
    offset += 2;
    *(uint32_t*)(data + offset) = 0;
    offset += 4;
    *(uint32_t*)(data + offset) = imageSize;
    offset += 4;
    *(int32_t*)(data + offset) = 2835;
    offset += 4;
    *(int32_t*)(data + offset) = 2835;
    offset += 4;
    *(uint32_t*)(data + offset) = 4;
    offset += 4;
    *(uint32_t*)(data + offset) = 4;
    offset += 4;
    
    // Палитра (4 уровня серого)
    // Формат: B, G, R, A
    uint32_t palette[] = {
        0x00000000, // 0 - Черный
        0x55555500, // 1 - Темно-серый
        0xAAAAAA00, // 2 - Светло-серый
        0xFFFFFFFF  // 3 - Белый
    };
    
    for (int i = 0; i < 4; i++)
    {
        uint32_t color = palette[i];
        data[offset++] = (color >> 0) & 0xFF;  // B
        data[offset++] = (color >> 8) & 0xFF;  // G
        data[offset++] = (color >> 16) & 0xFF; // R
        data[offset++] = (color >> 24) & 0xFF; // A
    }
    
    // Заполняем остальные цвета палитры градиентом
    for (int i = 4; i < 256; i++)
    {
        uint8_t gray = (i - 4) * 255 / 252;
        data[offset++] = gray;
        data[offset++] = gray;
        data[offset++] = gray;
        data[offset++] = 0;
    }
    
    // Данные пикселей
    // Буфер: каждый байт содержит 4 пикселя (по 2 бита)
    // Бит 7-6: пиксель 0, бит 5-4: пиксель 1, бит 3-2: пиксель 2, бит 1-0: пиксель 3
    // Страница: 4 строки (COM0-COM3)
    
    int pixelDataOffset = 14 + 40 + paletteSize;
    
    for (int row = 0; row < _height; row++)
    {
        // Определяем страницу (каждая страница = 4 строки)
        int page = row / 4;
        // Определяем строку внутри страницы (0-3)
        int rowInPage = row % 4;
        
        // Битовая позиция для 2-битного пикселя
        int bitShift = rowInPage * 2;
        
        for (int col = 0; col < _width; col++)
        {
            // Индекс в буфере: страница * ширина + колонка
            int bufferIndex = page * _width + col;
            
            // Извлекаем 2-битное значение
            int pixelValue = (_displayBuffer[bufferIndex] >> bitShift) & 0x03;
            
            // Записываем в BMP (8 бит на пиксель)
            int destPos = row * rowSize + col;
            data[pixelDataOffset + destPos] = pixelValue;
        }
    }
    
    return result;
}

// Обработчик корневой страницы
void DisplayWebServer::handleRoot()
{
    String html = getDisplayPageHTML();
    _server->send(200, "text/html", html);
}

// Обработчик изображения
void DisplayWebServer::handleDisplayImage()
{
    if (!_hasBuffer || !_displayBuffer)
    {
        _server->send(404, "text/plain", "No display buffer available");
        return;
    }
    
    std::vector<uint8_t> bmpData = convertToBMP();
    if (bmpData.empty())
    {
        _server->send(500, "text/plain", "Failed to convert display buffer");
        return;
    }
    
    _server->send(200, "image/bmp", String((char*)bmpData.data(), bmpData.size()));
}

// Обработчик статуса
void DisplayWebServer::handleStatus()
{
    String json = "{";
    json += "\"status\":\"" + _lastStatus + "\",";
    json += "\"buffer\":" + String(_hasBuffer ? "true" : "false") + ",";
    json += "\"width\":" + String(_width) + ",";
    json += "\"height\":" + String(_height) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += "}";
    _server->send(200, "application/json", json);
}

// Обработчик 404
void DisplayWebServer::handleNotFound()
{
    _server->send(404, "text/html",
                  "<html><body><h1>404</h1><p>Page not found</p></body></html>");
}

// Вспомогательные функции
String DisplayWebServer::getBytes(uint64_t bytes)
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