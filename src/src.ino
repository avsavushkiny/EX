#include "ex.h"

// === Функция задачи: показ формы с текстом ===
void showHelloForm()
{
    // Проверка: не открыта ли уже эта форма?
    if (!formsStack.empty())
    {
        exForm *top = formsStack.top();
        if (top->title == "Hello")
        {
            return; // уже открыта
        }
    }

    // Создаём новую форму
    exForm *form = new exForm();
    form->title = "Hello";
    form->eFormShowMode = NORMAL;           // оконный режим
    form->eFormBackground = LIGHT_GRAY;     // светлый фон

    // === Добавляем элементы ===

    // Заголовок
    form->addElement(new eLabel("Добро пожаловать!", 10, 20));

    // Подзаголовок
    form->addElement(new eLabel("Это первая форма", 10, 40));

    // Длинный текст
    form->addElement(new eLabel("Система готова.\n"
                                "Нажмите 'Close' \n"
                                "для выхода.", 10, 60));

    // Кнопка "Закрыть"
    // form->addElement(new eButton("Close", []()
    // {
    //     formsStack.pop();   // убираем из стека
    //     delete form;        // удаляем из памяти
    // }, 10, 110));

    // Добавляем форму в стек
    formsStack.push(form);
}


void setup()
{
    initializationSystem();
    Serial.begin(115200);

    // === Инициализация диспетчера задач ===
    _TD.init();

    // === Создаём задачу ===
    TaskArguments helloTask = {
        "HelloForm",          // имя
        showHelloForm,        // функция
        _ICON.app_wizard,              // bitmap (нет)
        USER,                 // тип
        100,                  // индекс
        true                  // активна
    };

    // Добавляем задачу
    _TD.addTask(helloTask);

    // Запускаем диспетчер (в отдельной FreeRTOS-задаче)
    // _TD.terminal();

    // === Запускаем форму сразу ===
    // _TD.runTask("HelloForm");
}

void loop()
{
    _TD.terminal();
}