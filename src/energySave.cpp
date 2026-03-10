#include "energySave.h"
#include "taskDispatcher.h"
#include "ui.h"


// Определение глобальных переменных
esp_timer_handle_t sleep_timer = nullptr;
volatile bool sleep_timeout = false;
const uint64_t SLEEP_TIMEOUT_US = 60000000; // 60 секунд в микросекундах

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
        .name = "sleep_timer"};

    esp_timer_create(&timer_args, &sleep_timer);
}

// Запуск/перезапуск таймера
void resetSleepTimer()
{
    if (sleep_timer == nullptr)
    {
        initSleepTimer();
    }

    // Останавливаем текущий таймер (если работает)
    esp_timer_stop(sleep_timer);

    // Сбрасываем флаг
    sleep_timeout = false;

    // Запускаем таймер заново
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

// Освобождение ресурсов таймера
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

bool isTouched()
{
    // Проверяем текущее состояние джойстика/кнопок
    bool touched = ((_JOY.calculateIndexY0() == 0) && (_JOY.calculateIndexX0() == 0)) ? false : true;

    // Если было касание - сбрасываем таймер сна
    if (touched)
    {
        resetSleepTimer();
    }

    return touched;
}

void energySave()
{
    static bool sleepMessageShown = false;

    // Инициализация таймера при первом вызове
    static bool timerInitialized = false;
    if (!timerInitialized)
    {
        initSleepTimer();
        resetSleepTimer(); // Запускаем таймер при старте
        setupWakeupGPIO(); // Настраиваем GPIO для пробуждения
        timerInitialized = true;
    }

    // Проверяем, было ли взаимодействие
    bool touched = isTouched();

    if (touched)
    {
        // Было взаимодействие - сбрасываем флаг сообщения
        sleepMessageShown = false;
        return;
    }

    // Проверяем таймаут сна
    if (sleep_timeout)
    {
        if (!sleepMessageShown)
        {
            // Показываем сообщение о переходе в спящий режим
            String text = "Entering sleep mode...\nMove joystick to wake up";
            InstantMessage message(text, 2000);
            message.show();
            sleepMessageShown = true;

            // Даем время прочитать сообщение
            delay(2100);
        }

        // Serial.println("Preparing for light sleep...");

        // Сохраняем состояние дисплея
        _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);

        // Останавливаем задачи перед сном
        stopSleepTimer();

        // Останавливаем системный таймер диспетчера задач
        _TD.stopHardwareTimer();

        // Serial.println("Entering light sleep...");

        // Входим в легкий сон
        esp_light_sleep_start();

        // Код выполнится после пробуждения
        // Serial.println("Woke up from light sleep!");

        // Восстанавливаем системный таймер
        _TD.initHardwareTimer();

        // Сбрасываем флаги и перезапускаем таймер
        sleep_timeout = false;
        sleepMessageShown = false;
        resetSleepTimer();

        // Обновляем состояние джойстика после пробуждения
        _JOY.updatePositionXY(20);

        // Принудительно обновляем дисплей
        // _GGL.gray.update();
    }
}

void forceSleep()
{
    String text = "Entering sleep mode...";
    InstantMessage message(text, 2000);
    message.show();

    delay(2100);

    // Настраиваем пробуждение
    setupWakeupGPIO();

    // Останавливаем системный таймер
    _TD.stopHardwareTimer();

    // Легкий сон
    _GGL.gray.setPowerMode(_GGL.gray.OPERATING_MODE);
    esp_light_sleep_start();

    // После пробуждения
    _TD.initHardwareTimer();
    resetSleepTimer();
}

void initSleepTimerTask()
{
    // Инициализация таймера сна
    initSleepTimer();
    resetSleepTimer();
    setupWakeupGPIO();
}