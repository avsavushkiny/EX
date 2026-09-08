#include "energySave.h"
#include "taskDispatcher.h"
#include "ui.h"

extern int runExFormStack();

// Глобальные переменные
esp_timer_handle_t sleep_timer = nullptr;
volatile bool sleep_timeout = false;
const uint64_t SLEEP_TIMEOUT_US = 20000000; // 60 секунд
static bool energy_save_enabled = true;
static bool is_sleeping = false;

// Callback функция таймера
void sleep_timer_callback(void *arg)
{
    sleep_timeout = true;
}

// Инициализация аппаратного таймера
void initSleepTimer()
{
    if (sleep_timer != nullptr)
        return;

    esp_timer_create_args_t timer_args = {
        .callback = &sleep_timer_callback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "sleep_timer"
    };

    esp_timer_create(&timer_args, &sleep_timer);
}

// Запуск/перезапуск таймера
void resetSleepTimer()
{
    if (sleep_timer == nullptr)
    {
        initSleepTimer();
    }

    esp_timer_stop(sleep_timer);
    sleep_timeout = false;
    esp_timer_start_once(sleep_timer, SLEEP_TIMEOUT_US);
}

// Остановка таймера
void stopSleepTimer()
{
    if (sleep_timer != nullptr)
    {
        esp_timer_stop(sleep_timer);
        sleep_timeout = false;
    }
}

// Освобождение ресурсов
void deleteSleepTimer()
{
    if (sleep_timer != nullptr)
    {
        esp_timer_stop(sleep_timer);
        esp_timer_delete(sleep_timer);
        sleep_timer = nullptr;
        sleep_timeout = false;
    }
}

// Настройка GPIO для пробуждения
void setupWakeupGPIO()
{
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1); // Stick 0
}

// Проверка активности пользователя
bool isTouched()
{
    static unsigned long last_check = 0;
    
    // Проверяем не чаще чем каждые 10 мс
    if (millis() - last_check < 10)
        return false;
    last_check = millis();
    
    // Проверяем состояние джойстика
    int x = _JOY.calculateIndexX0();
    int y = _JOY.calculateIndexY0();
    bool touched = (x != 0 || y != 0);
    
    // Также проверяем кнопки
    bool button_pressed = (_JOY.pressKeyENTER() || 
                          _JOY.pressKeyEX() || 
                          _JOY.pressKeyA() || 
                          _JOY.pressKeyB());
    
    touched = touched || button_pressed;

    // Если была активность - сбрасываем таймер
    if (touched)
    {
        resetSleepTimer();
    }

    return touched;
}

// Проверка возможности перехода в сон
bool canEnterSleep()
{
    // Не засыпаем если:
    // 1. Режим энергосбережения выключен
    if (!energy_save_enabled)
        return false;
    
    // 2. Уже спим
    if (is_sleeping)
        return false;
    
    // 3. Идет обновление по OTA
    // if (isOtaMode)
    //     return false;
    
    // 4. Есть активные задачи (можно добавить проверку)
    // if (_TD.getActiveTasksCount() > 0)
    //     return false;
    
    return true;
}

// Основная функция энергосбережения
void energySave()
{
    static bool timerInitialized = false; 
    static bool sleepMessageShown = false;
    
    if (!timerInitialized)
    {
        initSleepTimer();
        resetSleepTimer();
        setupWakeupGPIO();
        timerInitialized = true;
    }

    // Проверяем активность ПЕРЕД проверкой таймаута
    bool touched = isTouched();
    if (touched) 
    {
        sleepMessageShown = false;
        return;
    }

    // Защита от мгновенного повторного сна (Dead-time 1 секунда)
    static unsigned long lastWakeUpTime = 0;
    if (millis() - lastWakeUpTime < 1000) return; 

    if (sleep_timeout && canEnterSleep())
    {
        if (!sleepMessageShown)
        {
            String text = "Entering sleep mode...\nMove joystick to wake up";
            InstantMessage message(text, 1);
            message.show();
            sleepMessageShown = true;
        }

        is_sleeping = true;
        
        // Сохраняем количество тиков ДО ухода в сон для расчета дельты при пробуждении
        _TD.preSleepHardwareTicks = _TD.getHardwareTicks(); 
        
        // Останавливаем таймеры ESP-IDF, чтобы они не будили чип зря
        stopSleepTimer(); 
        _TD.stopHardwareTimer();
        
        // ВАЖНО: Выключаем питание периферии ДО сна, если драйвер поддерживает это без deinit
        // _GGL.gray.setPowerMode(_GGL.gray.SLEEP_MODE); 

        esp_light_sleep_start();

        // --- ПРОБУЖДЕНИЕ ---

        // Фиксируем время выхода из сна для dead-time
        lastWakeUpTime = millis(); 
        
        is_sleeping = false;
        
        // КРИТИЧЕСКИ ВАЖНО: Перезапускаем аппаратный тикер диспетчеризатора.
        _TD.initHardwareTimer(); 
        
        // Синхронизируем внутренние часы с реальностью. 
        // Эта функция увидит огромную дельту hardwareTicks и сбросит nextRunTime всех задач на +1 тик.
        
        // Альтернатива handleTimeSync (если она отсутствует), жестко задающая следующее выполнение:
        // _TD.resetSystemClock(); 

        // Включаем периферию обратно
        _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
        _GGL.gray.begin(); 
        
        // Очищаем артефакты старого кадра и перерисовываем актуальный интерфейс
        _GGL.gray.clearBuffer(); 
        runExFormStack(); 

        // Сбрасываем триггеры энергосбережения
        sleep_timeout = false;
        sleepMessageShown = false;
        
        // Возобновляем отсчет времени до следующего засыпания
        resetSleepTimer(); 
    }
}

// Принудительный сон
void forceSleep()
{
    if (!canEnterSleep()) return;

    _GGL.gray.clearBuffer();
    String text = "Entering sleep mode...";
    InstantMessage message(text, 1000);
    message.show();

    is_sleeping = true;
    setupWakeupGPIO();
    _TD.stopHardwareTimer();
    _GGL.gray.setPowerMode(_GGL.gray.SLEEP_MODE);

    esp_light_sleep_start();

    // --- ПРОБУЖДЕНИЕ ---
    is_sleeping = false;
    _TD.initHardwareTimer();
    _TD.resetSystemClock();

    _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
    _GGL.gray.begin(); _GGL.gray.setContrast(240);
    _GGL.gray.clearBuffer();
    runExFormStack(); // Обязательно выводим рабочий стол
    
    resetSleepTimer();
}

// Инициализация
void initSleepTimerTask()
{
    initSleepTimer();
    resetSleepTimer();
    setupWakeupGPIO();
    energy_save_enabled = true;
    is_sleeping = false;
    Serial.println("Sleep timer initialized");
}

// Управление режимом энергосбережения
void enableEnergySave()
{
    energy_save_enabled = true;
    resetSleepTimer();
    Serial.println("Energy save enabled");
}

void disableEnergySave()
{
    energy_save_enabled = false;
    stopSleepTimer();
    Serial.println("Energy save disabled");
}

bool isEnergySaveEnabled()
{
    return energy_save_enabled;
}