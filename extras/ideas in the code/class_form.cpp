/*
    Dev3 [not used]
    120125 - Aleksander Savushkin
    FormElement - Form with elements

    Abstract class constructor
*/
class FormElement0
{
public:
    FormElement0(const String &text, void (*f)(void), int x, int y) : m_text(text), m_f(f), m_x(x), m_y(y) {}

    virtual ~FormElement0() = default;
    virtual void display0() const = 0;

    void callFunction0()
    {
        m_f();
    }

private:
protected:
    String m_text;
    void (*m_f)(void);
    int m_x;
    int m_y;
};
/* Text message */
class TextMessage0 : public FormElement0
{
public:
    using FormElement0::FormElement0;

    void display0() const override
    {
    }

    // ex.cpp //void display() const override;
    /*
    void TextMessage0::display() const
    {
        _gfx.print(m_text, m_x, m_y);
    }
    */
};
/* Button */
class Button0 : FormElement0
{
public:
    using FormElement0::FormElement0;

    std::function<void()> onClick;

    void display0() const override
    {
        // output logic
    }
};
/* Checkbox */
class Checkbox0 : FormElement0
{
public:
    using FormElement0::FormElement0;

    bool isChecked = false;

    void display0() const override
    {
        // output logic
        if (isChecked)
        {
        }
        else
        {
        }
    }
};
namespace
{
    std::vector<FormElement0*> formElements0;
};
// adding elements
inline void addElement0(FormElement0* element)
{
    formElements0.push_back(element);
}
// displaying Forms
inline void displayFormElement0()
{
    u8g2.clearBuffer(); // -->
        for (auto& element : formElements0)
        {
            element->display0();
        }
    u8g2.sendBuffer(); // <--
}
/*
    Use Dev3
    
    // Create an instance of the TextMessage0 class
    TextMessage0* msg = new TextMessage0("Hello, World!", nullptr, 10, 20);
    
    // Adding an element to the list
    addElement0(msg);
    
    // Displaying all form elements
    displayFormElement0();
    
    // Clearing memory
    delete msg;
*/