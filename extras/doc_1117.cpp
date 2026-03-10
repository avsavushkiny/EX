// Документация по диспетчеру задач MOSAIC OS
// Диспетчер задач MOSAIC OS - это система управления задачами для микроконтроллеров,
// обеспечивающая планирование и выполнение задач с различными приоритетами и характеристиками.

// Архитектура
// Основные компоненты

// 1. Структура задачи (TaskArguments)
struct TaskArguments
{
    String name;               // Имя задачи
    void (*f)(void);           // Указатель на функцию задачи
    const uint8_t *bitMap;     // Иконка (опционально)
    TaskType type;             // Тип задачи (SYSTEM/DESKTOP/USER)
    int index;                 // Индекс задачи
    bool activ;                // Активность задачи
    TaskPriority priority;     // Приоритет выполнения
    bool oneShot;              // Одноразовая задача
    unsigned long lastRunTime; // Время последнего выполнения
    unsigned long interval;    // Интервал выполнения (в тиках)
    unsigned long nextRunTime; // Время следующего выполнения
};

// 2. Типы задач
// SYSTEM - Системные задачи (высокий приоритет)
// DESKTOP - Задачи рабочего стола
// USER - Пользовательские задачи

// 3. Уровни приоритета
enum TaskPriority
{
    PRIORITY_LOW = 0,      // Низкий приоритет
    PRIORITY_NORMAL = 1,   // Обычный приоритет
    PRIORITY_HIGH = 2,     // Высокий приоритет
    PRIORITY_CRITICAL = 3  // Критический приоритет
};

// Использование
// Инициализация диспетчера задач

// Глобальные векторы задач
extern std::vector<TaskArguments> tasks;
extern std::vector<TaskArguments> userTasks;

// Создание экземпляра диспетчера
TaskDispatcher _TD;
// Создание задач
// Вспомогательная функция для создания задач:

TaskArguments createTask(String name, void (*f)(void), const uint8_t *bitMap, 
                        TaskType type, int index, bool activ, 
                        TaskPriority priority = PRIORITY_NORMAL, 
                        bool oneShot = false, unsigned long interval = 1);

// Примеры создания задач:
// Одноразовая задача с высоким приоритетом
createTask("init", &initFunction, NULL, SYSTEM, 0, true, PRIORITY_HIGH, true)
// Периодическая задача с интервалом
createTask("sensor", &readSensor, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, false, 10)
// Критическая задача
createTask("emergency", &emergencyHandler, NULL, SYSTEM, 0, true, PRIORITY_CRITICAL, false, 1)
// Обычная задача рабочего стола
createTask("form1", &_myForm1, _ICON.window_abc, DESKTOP, 0, false, PRIORITY_NORMAL)

// Основные методы диспетчера
// Управление задачами
// Добавление задачи
_TD.addTask(task);
// Удаление задачи по имени
_TD.removeTask("task_name");
// Удаление задачи по индексу
_TD.removeTaskIndex(100);
// Активация задачи
_TD.runTask("task_name");
// Получение количества задач
int count = _TD.sizeTasks();

// Запуск системы
// Основной цикл выполнения
_TD.tick();
// Или через терминал (с отладкой)
_TD.terminal();

// Примеры задач
// Системные задачи
// Курсор (низкий приоритет для плавности)
createTask("cursor", &_systemCursor, NULL, SYSTEM, 0, true, PRIORITY_LOW, false, 1)
// Рабочий стол (обычный приоритет)
createTask("desktop", &_myDesktop, NULL, SYSTEM, 100, true, PRIORITY_NORMAL, false, 1)
// Стек форм (обычный приоритет)
createTask("stackform", &runExFormStack, NULL, SYSTEM, 0, true, PRIORITY_NORMAL, false, 1)