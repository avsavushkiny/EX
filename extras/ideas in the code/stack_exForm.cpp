// класс StackOfForms, который будет хранить указатели на объекты типа exForm.
#include <stack>

class StackOfForms // Класс для управления стеком форм
{
public:
    void push(exForm* form) // Добавить форму в стек
    {
        stack.push(form);
    }

    exForm* pop() // Извлечь верхнюю форму из стека
    {
        if (!stack.empty())
        {
            exForm* top = stack.top();
            stack.pop(); return top;
        }
        return nullptr;
    }

    size_t size() const // Получить количество форм в стеке
    {
        return stack.size();
    }

    bool empty() const // Проверка, пуст ли стек
    {
        return stack.empty();
    }

private:
    std::stack<exForm*> stack;
};

int main()
{
    StackOfForms formsStack;
    
    exForm* form1 = new exForm(); // Создаем несколько экземпляров классов exForm
    exForm* form2 = new exForm();
    exForm* form3 = new exForm();

    formsStack.push(form1); // Добавляем формы в стек
    formsStack.push(form2);
    formsStack.push(form3);

    std::cout << "Количество форм в стеке: " << formsStack.size() << std::endl;

    while (!formsStack.empty()) // Извлекаем формы из стека
    {
        exForm* currentForm = formsStack.pop();
        currentForm->showForm("Форма"); delete currentForm;
    }

    return 0;
}