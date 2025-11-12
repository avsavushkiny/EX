#include "ui.h"

bool Cursor::cursor(bool stateCursor, int xCursor, int yCursor)
{
    if (stateCursor == true)
    {
        _GGL.gray.bitmap(xCursor, yCursor, _SICON.cursor0, _SICON.cursor0_w, _SICON.cursor0_h, _GGL.gray.TRANSPARENT);
        
        return true;
    }
    else
        return false;
}
/* displaying a shortcut to a task-function */
bool Shortcut::shortcut(String name, const uint8_t *bitMap, uint8_t x, uint8_t y, void (*f)(void), int xCursor, int yCursor)
{
  _GGL.gray.bitmap(x, y, bitMap, 32, 32, _GGL.gray.NOT_TRANSPARENT);

  _GGL.gray.bitmap(x, y + 21, _SICON.shortcut0, 11, 11, _GGL.gray.NOT_TRANSPARENT);

//   TextBox textBoxNameTask;

  if ((xCursor >= x && xCursor <= (x + 32)) && (yCursor >= y && yCursor <= (y + 32)))
  {
    _GGL.gray.drawFrame(x, y, 32, 32, _GGL.gray.BLACK);

    // textBoxNameTask.textBox(name, 16, 32, 8, 5, x, y + 24);
    
    if (Joystick::pressKeyENTER() == true)
    {
      _GRF.waitDisplay();
      f();
      return true;
    }
  }
  else
  {
    // textBoxNameTask.textBox(name, 16, 32, 8, 5, x, y + 24);
  }

  return false;
}
/* [for Desktop name tasks] no Frame */
void TextBox::textBox(String str, int sizeH, int sizeW, short charH, short charW, int x, int y)
{
    short border{0}; short border2{0}; // size border
    int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
    int line{1}; // there will always be at least one line of text in the text
    int numberOfCharacters{0}; // количество символов
    int ch{0}, ln{0}; int xx{x}, yy{y};  
    
    for (char c : str)
    {
        numberOfCharacters++;
    }

    int numberOfCharactersLineFrame = (sizeW - border - border) / charW; // количество символов в строчке Фрейма
    int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
    int numberOfLinesFrame = (sizeH - border - border) / charH; // количество строчек в Фрейме
    
    for (char c : str)
    {
        _GGL.gray.writeChar(xx + border, yy + charH + border, c, 10, 1, _GGL.gray.BLACK);
        
        xx += charW;
        ch++;

        if (ch >= numberOfCharactersLineFrame)
        {
            yy += charH; ch = 0; xx = x; ln++;
        }

        if (ln >= numberOfLinesFrame)
        {
            break;
        }
    }
}
/* button return boolean state */
bool Button::button(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor)
{
  uint8_t sizeText = text.length(); short border{3}; short charW{5};

  if ((xCursor >= x && xCursor <= (x + (sizeText * charW) + border + border)) && (yCursor >= y && yCursor <= y + 13))
  {
    _GGL.gray.drawFillFrame(x, y, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.BLACK);
    _GGL.gray.writeLine(x + border, y - 1/* font H */ + border, text, 10, 1, _GGL.gray.WHITE);

    if (Joystick::pressKeyENTER() == true)
    {
      return true;
    }
  }
  else
  {
    _GGL.gray.drawFillFrame(x, y, (sizeText * charW) + border + border, 13, _GGL.gray.BLACK, _GGL.gray.WHITE);
    _GGL.gray.writeLine(x + border, y - 1/* font H */ + border, text, 10, 1, _GGL.gray.BLACK);
  }
  
  return false;
}
/* Draw show function */
void InstantMessage::show()
{
    short border{5}, border2{8}, charHeight{10}, charWidth{5};
    int count{0}, x{128}, y{80}, maxChar{0}, line{1};

    for (char c : m_text) // Считаем символы в каждой строке
    {
        if (c == '\n')
        {
            if (count > maxChar)
                maxChar = count;
            count = 0;
            line++;
        }
        else
        {
            count++;
        }
    }

    // Проверяем последнюю строку, если она не заканчивается на '\n'
    if (count > maxChar)
    {
        maxChar = count;
    }

    if (maxChar == 0 || line == 0) {
        return; // Не отображаем ничего, если нет символов или строк
    }

    int lineYoffset = (line / 2) * 8;
      
    //--> выводим границы текста
    int numberOfPixels = maxChar * charWidth;               // Максимальное количество пикселей на основе количества символов в строке
    int numberOfPixelsToOffset = (maxChar / 2) * charWidth; // Смещение для центрирования текста

    //--> границы фрейма
    int frameX = x - numberOfPixelsToOffset - border2;
    int frameY = y - charHeight - border2 - lineYoffset; //+?
    int frameWidth = border2 + border2 + numberOfPixels;
    int frameHeight = border2 + border2 + (line * charHeight);
    //<-- границы фрема

    // Рисуем заполненную рамку
    _GGL.gray.drawFillFrame(x - numberOfPixelsToOffset - border2,
        y - border2 - lineYoffset, border2 + border2 + numberOfPixels, 
        border2 + border2 + (line * charHeight), 
        _GGL.gray.WHITE, 
        _GGL.gray.WHITE
    );
    
    // Выводим текст
    _GRF.print(m_text, x - numberOfPixelsToOffset, y - lineYoffset, charHeight, charWidth);

    // Рисуем внешнюю рамку
    _GGL.gray.drawFrame(x - numberOfPixelsToOffset - border, 
        y - border - lineYoffset, 
        border + border + numberOfPixels, 
        border + border + (line * charHeight), 
        _GGL.gray.BLACK
    );

    // Тень
    _GGL.gray.drawHLine(
        x - numberOfPixelsToOffset - border + 3, // X: начало рамки
        y - lineYoffset + border + (line * charHeight), // Y: нижняя граница рамки
        border + numberOfPixels + border, // Длина: ширина рамки
        _GGL.gray.DARK_GRAY, 3
    );

    _GGL.gray.drawVLine(
        x - numberOfPixelsToOffset - border + (border + border + numberOfPixels), // X: начало рамки + ширина рамки + смещение
        y - border - lineYoffset + 3, // Y: начало рамки
        (line * charHeight) + border + border, // Высота: высота рамки
        _GGL.gray.DARK_GRAY, 3
    );

    _GGL.gray.sendBuffer();  // <--

    if (m_delay > 0) {
        delay(m_delay);
    }
}
/* starting a task-function with an interval */
void Timer::timer(void (*f)(void), int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        f();
    }
}
/* starting a task-function with an interval */
void Timer::timer(int (*f)(void), int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        f();
    }
}
/* return value */
bool Timer::timer(int interval)
{
    unsigned long currTime = millis();
    if (currTime - prevTime >= interval)
    {
        prevTime = currTime;
        return 1;
    }
    return 0;
}