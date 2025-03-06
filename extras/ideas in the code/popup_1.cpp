class exForm : public eForm
{
public:
    int showForm() override;

    // Метод для отображения моментального сообщения
    void showMessage(const String& message, int width = 200, int height = 100);

    String title = "Title form";
    EFORMSHOWMODE eFormShowMode;

private:
    int xForm, yForm;
    short outerBoundaryForm{20};

    // Вспомогательный метод для отображения окна с сообщением
    void drawMessageBox(const String& message, int x, int y, int width, int height);
};

// Реализация метода showMessage
void exForm::showMessage(const String& message, int width, int height)
{
    // Вычисляем позицию для отображения сообщения (центр экрана)
    int screenWidth = 480; // Пример ширины экрана
    int screenHeight = 320; // Пример высоты экрана
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    // Отрисовываем окно с сообщением
    drawMessageBox(message, x, y, width, height);
}

// Реализация метода drawMessageBox
void exForm::drawMessageBox(const String& message, int x, int y, int width, int height)
{
    // Отрисовка фона окна
    u8g2.setDrawColor(1);
    u8g2.drawBox(x, y, width, height);

    // Отрисовка текста сообщения
    u8g2.setDrawColor(0);
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(x + 10, y + 20, message.c_str());

    // Отрисовка кнопки "OK"
    u8g2.drawButtonUTF8(x + width - 60, y + height - 10, U8G2_BTN_INV | U8G2_BTN_BW1, 50, 1, 1, "OK");

    // Обновление экрана
    u8g2.sendBuffer();
}

// Реализация метода showForm
int exForm::showForm()
{
    // Отрисовка основного окна формы
    // ...

    // Пример использования showMessage
    showMessage("This is a message!", 200, 100);

    return 0;
}

// Пример
// Основная программа
int main() {
    // Создаем форму
    exForm form;
    form.title = "Main Form";
    form.eFormShowMode = NORMAL;

    // Создаем кнопку
    eButton button("Click Me", []() {
        std::cout << "Button clicked!" << std::endl;
    }, 100, 50);

    // Добавляем кнопку на форму
    form.addElement(&button);

    // Отображаем форму
    form.showForm();

    // Пример вызова моментального сообщения
    form.showMessage("Hello, this is a message!", 200, 100);

    return 0;
}

// Дополнительные сценарии:
// Обработка нажатия кнопки:
// Вы можете изменить обработчик кнопки, чтобы при нажатии на кнопку отображалось сообщение:

eButton button("Click Me", [&form]() {
    form.showMessage("Button clicked!", 200, 100);
}, 100, 50);

// Динамическое создание сообщений:
// Вы можете создавать сообщения динамически в зависимости от условий:

if (someCondition) {
    form.showMessage("Condition met!", 200, 100);
}

// Кастомизация сообщений:
// Вы можете передавать разные параметры в метод showMessage, чтобы изменять размер, текст и другие атрибуты окна:

form.showMessage("Warning!", 300, 150);