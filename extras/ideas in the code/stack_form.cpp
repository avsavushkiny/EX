#include <stack>

class FormStack
{
public:
    void push(exForm& form)
    {
        forms.push(&form);
    }

    void pop()
    {
        if (!forms.empty())
        {
            forms.pop();
        }
    }

    exForm* top()
    {
        if (!forms.empty())
        {
            return forms.top();
        }
        return nullptr;
    }

private:
    std::stack<exForm*> forms;
};


FormStack formStack;
exForm form1;
exForm form2;

formStack.push(form1);
formStack.push(form2);

exForm* currentForm = formStack.top();
if (currentForm != nullptr)
{
    currentForm->showForm();
}
