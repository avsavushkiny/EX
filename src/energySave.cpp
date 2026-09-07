#include "energySave.h"
#include "taskDispatcher.h"
#include "ui.h"

extern int runExFormStack();

// Глобальные переменные
esp_timer_handle_t sleep_timer = nullptr;
volatile bool sleep_timeout = false;
const uint64_t SLEEP_TIMEOUT_US = 60000000; // 60 секунд
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
    // // Настройка пинов джойстика для пробуждения
    // const gpio_num_t wakeup_pins[] = {
    //     GPIO_NUM_32, // Джойстик X
    //     GPIO_NUM_33, // Джойстик Y (если используется)
    // };

    // for (gpio_num_t pin : wakeup_pins)
    // {
    //     gpio_set_direction(pin, GPIO_MODE_INPUT);
    //     gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    //     gpio_wakeup_enable(pin, GPIO_INTR_ANYEDGE);
    // }

    // // Также добавляем кнопки для пробуждения
    // const gpio_num_t button_pins[] = {
    //     GPIO_NUM_14, // EX button
    //     GPIO_NUM_27, // ENTER button (если используется)
    // };

    // for (gpio_num_t pin : button_pins)
    // {
    //     gpio_set_direction(pin, GPIO_MODE_INPUT);
    //     gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    //     gpio_wakeup_enable(pin, GPIO_INTR_ANYEDGE);
    // }

    // esp_sleep_enable_gpio_wakeup();
    
    // Настройка пинов джойстика/кнопок для пробуждения
    const gpio_num_t wakeup_pins[] = {
        GPIO_NUM_32, // Джойстик
    };

    for (gpio_num_t pin : wakeup_pins)
    {
        // Настройка GPIO как вход с подтяжкой к питанию
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);

        // Включение пробуждения по изменению уровня
        gpio_wakeup_enable(pin, GPIO_INTR_ANYEDGE);
    }

    // Включение пробуждения по GPIO
    esp_sleep_enable_gpio_wakeup();
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
    static bool sleepMessageShown = false;
    static bool timerInitialized = false;
    
    // Инициализация при первом вызове
    if (!timerInitialized)
    {
        initSleepTimer();
        resetSleepTimer();
        setupWakeupGPIO();
        timerInitialized = true;
        Serial.println("Energy save system initialized");
    }

    // Проверяем активность
    bool touched = isTouched();
    
    if (touched)
    {
        sleepMessageShown = false;
        return;
    }

    // Проверяем таймаут и возможность сна
    if (sleep_timeout && canEnterSleep())
    {
        if (!sleepMessageShown)
        {
            String text = "Entering sleep mode...\nMove joystick to wake up";
            InstantMessage message(text, 2000);
            message.show();
            sleepMessageShown = true;
        }

        is_sleeping = true;
        
        // Сохраняем состояние дисплея
        _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
        
        // Останавливаем таймеры
        stopSleepTimer();
        _TD.stopHardwareTimer();
        
        // Выключаем подсветку если есть
        // digitalWrite(PIN_BACKLIGHT_LCD, LOW);
        
        // Входим в сон
        esp_light_sleep_start();
        
        // --- ПРОБУЖДЕНИЕ ---
        is_sleeping = false;
        
        // Восстанавливаем таймеры
        _TD.initHardwareTimer();
        
        // Включаем подсветку
        // digitalWrite(PIN_BACKLIGHT_LCD, HIGH);
        
        // Восстанавливаем дисплей
        _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
        _GGL.gray.clearBuffer();
        
        // Перерисовываем интерфейс
        runExFormStack();
        
        // Сбрасываем флаги
        sleep_timeout = false;
        sleepMessageShown = false;
        resetSleepTimer();
        
        // Обновляем состояние джойстика
        // _JOY.updatePositionXY();
    }
}

// Принудительный сон
void forceSleep()
{
    if (!canEnterSleep())
    {
        Serial.println("Cannot enter sleep mode now");
        return;
    }
    
    String text = "Entering sleep mode...";
    InstantMessage message(text, 2000);
    message.show();
    delay(2100);

    is_sleeping = true;
    
    setupWakeupGPIO();
    _TD.stopHardwareTimer();
    _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
    // digitalWrite(PIN_BACKLIGHT_LCD, LOW);
    
    esp_light_sleep_start();

    // Пробуждение
    is_sleeping = false;
    _TD.initHardwareTimer();
    _TD.resetSystemClock();

    // digitalWrite(PIN_BACKLIGHT_LCD, HIGH);
    _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
    // _GGL.gray.clearBuffer();
    // runExFormStack();
    resetSleepTimer();
    // _JOY.updatePositionXY();
}

// void forceSleep()
// {
//     if (!canEnterSleep())
//     {
//         Serial.println("Cannot enter sleep mode now");
//         return;
//     }
    
//     // Показываем сообщение пользователю
//     String text = "Entering sleep mode...\nMove joystick to wake up";
//     InstantMessage message(text, 2000);
//     message.show(); delay(2000);

//     is_sleeping = true;
    
//     // ВАЖНО: Настройка GPIO должна быть ДО вызова esp_light_sleep_start
//     setupWakeupGPIO(); 

//     // Останавливаем только высокочастотные фоновые процессы
//     _TD.stopHardwareTimer(); 
    
//     // Отключаем Wi-Fi/BT для экономии энергии (если они активны)
//     // if (WiFi.getMode() != WIFI_MODE_NULL) {
//     //     WiFi.mode(WIFI_OFF);
//     // }

//     // Входим в легкую спячку. 
//     // Эта функция блокирует поток до момента пробуждения по GPIO.
//     esp_light_sleep_start();

//     // --- КОД ПОСЛЕ ПРОБУЖДЕНИЯ ---
//     // ESP.restart();

//     is_sleeping = false;

//     // Проверяем причину выхода (защита от ложных срабатываний)
//     if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
//         Serial.println("Spurious wakeup detected.");
//     }

//     // КРИТИЧЕСКИ ВАЖНО: Восстанавливаем аппаратный таймер ПЕРЕД перерисовкой UI
//     _TD.initHardwareTimer(); _TD.resetSystemClock();

//     // Включаем беспроводные интерфейсы обратно, если они были нужны
//     // wifiManager.autoReconnect(); 

//     // Сбрасываем состояние энергосбережения
//     resetSleepTimer();
    
//     // Принудительно опрашиваем состояние джойстика один раз,
//     // чтобы обновить координаты курсора актуальными данными АЦП
//     _JOY.updatePositionXY(10); 

//     // Очищаем буфер экрана от артефактов сна
//     _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
//     _GGL.gray.clearBuffer();
    
//     // Перерисовываем интерфейс поверх чистого холста
//     runExFormStack();
// }

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