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

/* button return boolean state */
// bool Button::button(String text, uint8_t x, uint8_t y, uint8_t xCursor, uint8_t yCursor)
// {
//   uint8_t sizeText = text.length(); short border{3}; short charW{5};

//   if ((xCursor >= x && xCursor <= (x + (sizeText * charW) + border + border)) && (yCursor >= y && yCursor <= y + 13))
//   {
//     ggl.gray.drawFillFrame(x, y, (sizeText * charW) + border + border, 13, ggl.gray.BLACK, ggl.gray.BLACK);
//     ggl.gray.writeLine(x + border, y - 1/* font H */ + border, text, 10, 1, ggl.gray.WHITE);

//     if (Joystick::pressKeyENTER() == true)
//     {
//       return true;
//     }
//   }
//   else
//   {
//     ggl.gray.drawFillFrame(x, y, (sizeText * charW) + border + border, 13, ggl.gray.BLACK, ggl.gray.WHITE);
//     ggl.gray.writeLine(x + border, y - 1/* font H */ + border, text, 10, 1, ggl.gray.BLACK);
//   }
  
//   return false;
// }

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

// /* [for Desktop name tasks] no Frame */
// void TextBox::textBox(String str, int sizeH, int sizeW, short charH, short charW, int x, int y)
// {
//     short border{0}; short border2{0}; // size border
//     int count{0}; int countChars{0}; int maxChar{0}; // for counting characters
//     int line{1}; // there will always be at least one line of text in the text
//     int numberOfCharacters{0}; // количество символов
//     int ch{0}, ln{0}; int xx{x}, yy{y};  
    
//     for (char c : str)
//     {
//         numberOfCharacters++;
//     }

//     int numberOfCharactersLineFrame = (sizeW - border - border) / charW; // количество символов в строчке Фрейма
//     int numberOfLines = numberOfCharacters / numberOfCharactersLineFrame; // количество строк
//     int numberOfLinesFrame = (sizeH - border - border) / charH; // количество строчек в Фрейме
    
//     for (char c : str)
//     {
//         ggl.gray.writeChar(xx + border, yy + charH + border, c, 10, 1, ggl.gray.BLACK);
        
//         xx += charW;
//         ch++;

//         if (ch >= numberOfCharactersLineFrame)
//         {
//             yy += charH; ch = 0; xx = x; ln++;
//         }

//         if (ln >= numberOfLinesFrame)
//         {
//             break;
//         }
//     }
// }