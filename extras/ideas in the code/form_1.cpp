/*h*/
/*
    Form
    Visual Form Builder
    [01/2025, Alexander Savushkin]
*/
/* Basic interface for all form elements */
class FormElement
{
public:
    virtual void Show() const = 0;
};
/* Button */
class FormButton : public FormElement
{
public:
    FormButton(const String& label, void (*onClick)(), int x, int y) : m_label(label), m_onClick(onClick), m_x(x), m_y(y) {}

    void Show() const override;

    void Click() //delete const in fShow
    {
        if (m_onClick != nullptr)
            m_onClick();
    }

private:
    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};

    String m_label;
    void (*m_onClick)(void);
    int m_x;
    int m_y;
};
/* Text */
class FormText : public FormElement
{
public:
    FormText(const String& text, int x, int y) : m_text(text), m_x(x), m_y(y) {}

    void Show() const override;

private:
    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};

    short const highChar {10};

    String m_text;
    int m_x;
    int m_y;
};
/* Border style, text-box */
enum BorderStyle {noBorder, oneLine, twoLine, shadow, shadowNoFrame};
/* Text-box */
class FormTextBox : public FormElement
{
public:
    FormTextBox(const String& text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y) : m_text(text), m_borderStyle(borderStyle), m_sizeW(sizeW), m_sizeH(sizeH), m_x(x), m_y(y) {}

    void Show() const override;

private:
    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};

    BorderStyle m_borderStyle;
    String m_text;
    int m_x, m_y;
    int m_sizeW, m_sizeH;
};
/* Label to link */
class FormLabel : public FormElement
{
public:
    FormLabel(const String& text, void (*onClick)(), int x, int y) : m_text(text), m_onClick(onClick), m_x(x), m_y(y) {}

    void Show() const override;

private:
    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};

    void (*m_onClick)(void);
    String m_text;
    int m_x, m_y;
    int m_sizeW, m_sizeH;
};
/* Implementation of a concrete class exForm */
class Form
{
private:
    std::vector<FormElement*> m_elements;

    short const outerBoundaryForm {20};
    short const innerBoundaryForm {5};
public:
    ~Form()
    {
        for (FormElement* element : m_elements)
        {
            delete element;
        }
    }

    void addButton(const String& label, void (*onClick)(), int x, int y)
    {
        m_elements.push_back(new FormButton(label, onClick, x, y));
    }

    void addText(const String& text, int x, int y)
    {
        m_elements.push_back(new FormText(text, x, y));
    }

    void addTextBox(const String& text, BorderStyle borderStyle, int sizeW, int sizeH, int x, int y)
    {
        m_elements.push_back(new FormTextBox(text, borderStyle, sizeW, sizeH, x, y));
    }

    void addLabel(const String& text, void (*onClick)(), int x, int y)
    {
        m_elements.push_back(new FormLabel(text, onClick, x, y));
    }

    void showForm(const String& title) const;
};







/*cpp*/
/* Text */
void FormText::Show() const
{
    // 6px - offset by Y
    _gfx.print(m_text, outerBoundaryForm + m_x, outerBoundaryForm + 6 + highChar + m_y, 10, 5);
}
/* Button */
void FormButton::Show() const
{
    uint8_t sizeText = m_label.length();
    short border{3};
    short charW{5};

    int x{m_x + outerBoundaryForm}, y{m_y + outerBoundaryForm + 6 /* offset by Y */};


    if ((_joy.posX0 >= x && _joy.posX0 <= (x + (sizeText * charW) + border + border)) && (_joy.posY0 >= y && _joy.posY0 <= y + 13))
    {
        u8g2.drawBox(x, y, (sizeText * charW) + border + border, 13);
        if (_joy.pressKeyENTER() == true)
        {
            m_onClick(); 
        }
    }
    else
    {
        u8g2.drawFrame(x, y, (sizeText * charW) + border + border, 13);
    }

    u8g2.setFontMode(1);

    u8g2.setDrawColor(2);
    _gfx.print(m_label, x + border, y + 7 /* font H */ + border, 8, charW);
    u8g2.setDrawColor(1);

    u8g2.setFontMode(1);
}
/* Text-box */
void FormTextBox::Show() const
{
    short border{5}; short border2{8}; // size border
    int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
    short line{1}; // there will always be at least one line of text in the text
    int numberOfCharacters{0}; // количество символов
    short charH{10}, charW{5};
    int ch{0}, ln{0}; int xx{m_x + outerBoundaryForm}, yy{m_y + outerBoundaryForm + 6 /* offset by Y */}; 
    
    if (m_borderStyle == noBorder){ /* we don't draw anything */ }
    if (m_borderStyle == oneLine)
    {
        u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
    }
    if (m_borderStyle == twoLine)
    {
        u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);
        u8g2.drawFrame(xx - 3, yy - 3, m_sizeW + 6, m_sizeH + 6);
    }
    if (m_borderStyle == shadow)
    {
        u8g2.drawFrame(xx, yy, m_sizeW, m_sizeH);

        u8g2.drawHLine(xx + 1, yy + m_sizeH, m_sizeW);
        u8g2.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW);

        u8g2.drawVLine(xx + m_sizeW, yy + 1, m_sizeH);
        u8g2.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH);
    }
    if (m_borderStyle == shadowNoFrame)
    {
        u8g2.drawHLine(xx + 1, yy + m_sizeH, m_sizeW);
        u8g2.drawHLine(xx + 2, yy + m_sizeH + 1, m_sizeW);

        u8g2.drawVLine(xx + m_sizeW, yy + 1, m_sizeH);
        u8g2.drawVLine(xx + m_sizeW + 1, yy + 2, m_sizeH);
    }
    
    
    for (char c : m_text)
    {
        numberOfCharacters++;
    }

    int numberOfCharactersLineFrame = (m_sizeW - border - border) / charW; // количество символов в строчке Фрейма
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
    int numberOfLinesFrame = (m_sizeH - border - border) / charH; // количество строчек в Фрейме
    
    for (char c : m_text)
    {
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.setCursor(xx + border, yy + charH + border);
        u8g2.print(c);

        // adds dots if frame is full
        /*if ((ln >= numberOfLinesFrame - 1) && (numberOfCharactersLineFrame == (ch + 3)))
        {
            u8g2.print("...");
        }
        else u8g2.print(c);*/
        
        xx += charW;
        ch++;

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH; ch = 0; xx = m_x + outerBoundaryForm; ln++;
        }

        // if (ln >= numberOfLinesFrame)
        // {
        //     // draw glyph
        //     /*u8g2.setFont(u8g2_font_unifont_t_symbols);
        //     u8g2.drawGlyph(x + sizeW - 8, y + sizeH + 2, 0x2198);*/

        //     u8g2.drawTriangle(xx + m_sizeW, yy + m_sizeH - 8, xx + m_sizeW, yy + m_sizeH, xx + m_sizeW - 8, yy + m_sizeH);
        //     break;
        // }
    }
}
/* Label */
void FormLabel::Show() const
{
    uint8_t sizeText = m_text.length();
    uint8_t yy{}, chi{5}, lii{8}; int x{m_x + outerBoundaryForm}, y{m_y + outerBoundaryForm + 6 /* offset by Y */};

    if ((_joy.posX0 >= x && _joy.posX0 <= (x + (sizeText * chi))) && (_joy.posY0 >= y - (lii + 2) && _joy.posY0 <= y + 2))
    {

        u8g2.drawBox(x - 1, y - (lii), (sizeText * chi) + 2, lii + 1);

        if (_joy.pressKeyENTER() == true)
        {
            m_onClick();
        }
    }
    else
    {
        u8g2.setDrawColor(1);
    }

    u8g2.setCursor(x + 3, y);
    u8g2.setFont(u8g2_font_6x10_tr);
    
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);//2
    
    for (int i = 0, xx = 0; i < sizeText, xx < (sizeText * chi); i++, xx += chi)
    {
        u8g2.setCursor(xx + x, yy + y);
        u8g2.print(m_text[i]);

        if (m_text[i] == '\n')
        {
            yy += lii; // 10
            xx = -chi; // 6
        }
    }

    u8g2.setFontMode(0); //0-activate not-transparent font mode
}
/* Form show */
void Form::showForm(const String &title) const
{
    Button fClose;

    while (1)
    {
        u8g2.clearBuffer(); // -->

            if (fClose.button2("CLOSE", 205, outerBoundaryForm - 12 + 6, _joy.posX0, _joy.posY0))
            {
                break;
            }

            u8g2.drawFrame(outerBoundaryForm, outerBoundaryForm + 6, 216, 120); // x, y, w, h
            _gfx.print(10, title, outerBoundaryForm + 5, outerBoundaryForm - 1 + 6, 10, 5);

                for (const auto &element : m_elements)
                {
                    element->Show();
                }

            // cursor
            _joy.updatePositionXY(20);
            _crs.cursor(true, _joy.posX0, _joy.posY0);

        u8g2.sendBuffer(); // <--
    }
}
