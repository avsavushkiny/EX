/* Implementation of a concrete class exForm */
class exForm : public eForm
{
public:
    int showForm() const override;

    // Метод для изменения значения outerBoundaryForm у всех элементов
    void setOuterBoundaryForm(short newValue)
    {
        for (auto element : elements)
        {
            if (dynamic_cast<eButton*>(element))
                static_cast<eButton*>(element)->outerBoundaryForm = newValue;
            else if (dynamic_cast<eText*>(element))
                static_cast<eText*>(element)->outerBoundaryForm = newValue;
            else if (dynamic_cast<eTextBox*>(element))
                static_cast<eTextBox*>(element)->outerBoundaryForm = newValue;
            else if (dynamic_cast<eLabel*>(element))
                static_cast<eLabel*>(element)->outerBoundaryForm = newValue;
        }
    }

    String title = "Title form";
private:
    enum eFormShowMode { FULLSCREEN, MAXIMIZED, NORMAL };
    short const outerBoundaryForm{20};
};