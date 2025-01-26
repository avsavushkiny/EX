#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

// Определение типа строки
typedef string String;

// Определение стиля границы
enum BorderStyle { SOLID, DOTTED };

/**
 * Интерфейс для всех элементов формы
 */
class eElement
{
public:
    virtual ~eElement() {}
    virtual void show() const = 0;
    virtual bool canReceiveFocus() const { return false; }
    virtual void onFocus() {}
    virtual void onBlur() {}
};

/**
 * Класс для управления фокусом
 */
class FocusManager
{
public:
    void pushFocus(eElement* element)
    {
        if (!focusStack.empty())
            focusStack.top()->onBlur(); // Убираем фокус у предыдущего элемента

        focusStack.push(element);       // Добавляем новый элемент в стек
        element->onFocus();             // Устанавливаем фокус на новый элемент
    }

    void popFocus()
    {
        if (!focusStack.empty()) {
            focusStack.top()->onBlur(); // Убираем фокус у текущего элемента
            focusStack.pop();           // Удаляем текущий элемент из стека
        }

        if (!focusStack.empty())
            focusStack.top()->onFocus(); // Возвращаем фокус предыдущему элементу
    }

    eElement* getCurrentFocusedElement() const
    {
        if (focusStack.empty())
            return nullptr;
        else
            return focusStack.top();
    }

private:
    stack<eElement*> focusStack;
};

/**
 * Кнопка
 */
class eButton : public eElement
{
public:
    eButton(const String& label, void (*onClick)(), int x, int y) : m_label(label), m_onClick(onClick), m_x(x), m_y(y) {}

    void setLabel(const String& new_label)
    {
        m_label = new_label;
    }

    void show() const override
    {
        cout << "Button: " << m_label << endl;
    }

    bool canReceiveFocus() const override
    {
        return true;
    }

    void onFocus() override
    {
        cout << "Button focused: " << m_label << endl;
    }

    void onBlur() override
    {
        cout << "Button blurred: " << m_label << endl;
    }

private:
    String m_label;
    void (*m_onClick)(void);
    short const outerBoundaryForm{20};
    int m_x, m_y;
};

/**
 * Многострочный текст
 */
class eText : public eElement
{
public:
    eText(const String& text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

    void setText(const String& new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() const override
    {
        cout << "Multiline text: " << m_text << endl;
    }

private:
    String m_text;
    short const highChar{10};
    short const outerBoundaryForm{20};
    int m_x, m_y;
};

/**
 * Поле ввода текста
 */
class eTextBox : public eElement
{
public:
    eTextBox(const String& text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y) :
        m_text(text),
        m_borderStyle(borderStyle),
        m_sizeW(sizeW),
        m_sizeH(sizeH),
        m_x(x),
        m_y(y) {}

    void setText(const String& new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() const override
    {
        cout << "Textbox: " << m_text << endl;
    }

    bool canReceiveFocus() const override
    {
        return true;
    }

    void onFocus() override
    {
        cout << "Textbox focused: " << m_text << endl;
    }

    void onBlur() override
    {
        cout << "Textbox blurred: " << m_text << endl;
    }

private:
    BorderStyle m_borderStyle;
    String m_text;
    short const outerBoundaryForm{20};
    int m_x, m_y;
    int m_sizeW, m_sizeH;
};

/**
 * Метка-ссылка
 */
class eLabel : public eElement
{
public:
    eLabel(const String& text, void (*onClick)(), int x, int y) : m_text(text), m_onClick(onClick), m_x(x), m_y(y) {}

    void setText(const String& new_text)
    {
        m_text = new_text;
    }

    String getText() const
    {
        return m_text;
    }

    void show() const override
    {
        cout << "Label: " << m_text << endl;
    }

private:
    void (*m_onClick)(void);
    short const outerBoundaryForm{20};
    String m_text;
    int m_x, m_y;
    int m_sizeW, m_sizeH;
};

/**
 * Абстрактный базовый класс формы
 */
class eForm
{
public:
    virtual ~eForm() {}

    void addElement(eElement* element)
    {
        elements.push_back(element);
    }

    virtual void showForm(const String& title) const = 0;

protected:
    vector<eElement*> elements;
};

/**
 * Реализация конкретной формы
 */
class exForm : public eForm
{
public:
    void showForm(const String& title) const override
    {
        cout << "Form Title: " << title << endl;

        for (const auto& element : elements)
        {
            element->show();

            if (element->canReceiveFocus())
                focusManager.pushFocus(element);
        }
    }

private:
    FocusManager focusManager;
    short const outerBoundaryForm{20};
};

int main()
{
    exForm myForm;

    eButton button("Submit", nullptr, 100, 200);
    eTextBox textbox("", SOLID, 50, 30, 150, 250);
    eText multilineText("This is a multiline text.", 300, 350);

    myForm.addElement(&button);
    myForm.addElement(&textbox);
    myForm.addElement(&multilineText);

    myForm.showForm("My Form");

    return 0;
}