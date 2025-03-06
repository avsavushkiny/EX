// Чтобы сделать `ButtonClickHandler` универсальным классом, который может принимать указатель на любую функцию (например, `f1` или `f2`), можно использовать **указатели на функции** или **std::function**. В этом примере я покажу, как использовать `std::function`, так как это более гибкий и современный подход.

#include <iostream>
#include <vector>
#include <string>
#include <functional>

// Интерфейс наблюдателя
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onEvent(const std::string& eventName, void* data) = 0;
};

// Интерфейс субъекта
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers(const std::string& eventName, void* data) = 0;
};

// Реализация субъекта
class Subject : public ISubject {
public:
    void addObserver(IObserver* observer) override {
        observers.push_back(observer);
    }

    void removeObserver(IObserver* observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notifyObservers(const std::string& eventName, void* data) override {
        for (auto observer : observers) {
            observer->onEvent(eventName, data);
        }
    }

private:
    std::vector<IObserver*> observers;
};

// Пример элемента формы, который генерирует события (например, кнопка)
class eButton : public Subject {
public:
    eButton(const std::string& label, int x, int y) : m_label(label), m_x(x), m_y(y) {}

    void setLabel(const std::string &new_label) {
        m_label = new_label;
    }

    std::string getLabel() const {
        return m_label;
    }

    void click() {
        // Генерация события при нажатии на кнопку
        notifyObservers("button_click", this);
    }

private:
    std::string m_label;
    int m_x{0}, m_y{0};
};

// Функция, которую нужно вызвать по нажатию на первую кнопку
void f1() {
    std::cout << "Function f1 executed!" << std::endl;
}

// Функция, которую нужно вызвать по нажатию на вторую кнопку
void f2() {
    std::cout << "Function f2 executed!" << std::endl;
}

// Универсальный обработчик для кнопок
class ButtonClickHandler : public IObserver {
public:
    // Конструктор, принимающий std::function<void()>
    ButtonClickHandler(std::function<void()> callback) : m_callback(callback) {}

    void onEvent(const std::string& eventName, void* data) override {
        if (eventName == "button_click") {
            eButton* button = static_cast<eButton*>(data);
            std::cout << "Button clicked: " << button->getLabel() << std::endl;

            // Вызов переданной функции
            m_callback();
        }
    }

private:
    std::function<void()> m_callback; // Указатель на функцию
};

int main() {
    // Создаем две кнопки
    eButton button1("Button 1", 10, 10);
    eButton button2("Button 2", 50, 50);

    // Создаем универсальные обработчики для каждой кнопки
    ButtonClickHandler handler1(f1); // Передаем f1 в обработчик для первой кнопки
    ButtonClickHandler handler2(f2); // Передаем f2 в обработчик для второй кнопки

    // Подписываем обработчики на соответствующие кнопки
    button1.addObserver(&handler1);
    button2.addObserver(&handler2);

    // Симулируем нажатие на первую кнопку
    std::cout << "Clicking Button 1:" << std::endl;
    button1.click();

    // Симулируем нажатие на вторую кнопку
    std::cout << "\nClicking Button 2:" << std::endl;
    button2.click();

    return 0;
}

// Объяснение:

// 1. **Универсальный обработчик `ButtonClickHandler`**:
//    - Конструктор принимает `std::function<void()>`, который может хранить указатель на любую функцию, соответствующую сигнатуре `void()`.
//    - В методе `onEvent` вызывается переданная функция через `m_callback()`.

// 2. **Функции `f1` и `f2`**:
//    - Это обычные функции, которые не принимают аргументов и не возвращают значения (`void()`).

// 3. **Создание обработчиков**:
//    - `ButtonClickHandler handler1(f1);` — создает обработчик для первой кнопки, который вызывает `f1`.
//    - `ButtonClickHandler handler2(f2);` — создает обработчик для второй кнопки, который вызывает `f2`.

// 4. **Связывание**:
//    - Каждая кнопка подписывается на свой обработчик:
//      - `button1.addObserver(&handler1);`
//      - `button2.addObserver(&handler2);`

// 5. **Симуляция нажатий**:
//    - При вызове `button1.click()` вызывается `f1`.
//    - При вызове `button2.click()` вызывается `f2`.

// Вывод программы:

// Clicking Button 1:
// Button clicked: Button 1
// Function f1 executed!

// Clicking Button 2:
// Button clicked: Button 2
// Function f2 executed!


// ### Преимущества:
// - **Гибкость**: Вы можете передавать любую функцию с подходящей сигнатурой в обработчик.
// - **Универсальность**: Один класс `ButtonClickHandler` может использоваться для всех кнопок.
// - **Простота добавления новых функций**: Чтобы добавить новую функцию, просто передайте её в обработчик.

// ### Дополнительно:
// Если функции `f1` и `f2` должны принимать аргументы (например, метку кнопки), вы можете изменить сигнатуру `std::function` и передавать данные:

void f1(const std::string& label) {
    std::cout << "Function f1 executed! Button label: " << label << std::endl;
}

void f2(const std::string& label) {
    std::cout << "Function f2 executed! Button label: " << label << std::endl;
}

// Обработчик с поддержкой аргументов
class ButtonClickHandler : public IObserver {
public:
    ButtonClickHandler(std::function<void(const std::string&)> callback) : m_callback(callback) {}

    void onEvent(const std::string& eventName, void* data) override {
        if (eventName == "button_click") {
            eButton* button = static_cast<eButton*>(data);
            std::cout << "Button clicked: " << button->getLabel() << std::endl;

            // Вызов переданной функции с аргументом
            m_callback(button->getLabel());
        }
    }

private:
    std::function<void(const std::string&)> m_callback;
};

// Использование:
ButtonClickHandler handler1(f1);
ButtonClickHandler handler2(f2);

// Теперь функции `f1` и `f2` могут принимать метку кнопки в качестве аргумента.