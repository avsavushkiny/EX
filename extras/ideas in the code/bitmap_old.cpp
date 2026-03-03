void GRAY::bitmap(short x, short y, const uint8_t *pBmp, short chWidth, short chHeight)
{
  int i, j, k;
  int16_t yy = y * 2;          // Удваиваем координату y
  int page = chHeight * 2 / 8; // Вычисляем количество страниц
  if (page == 0)               // Если количество страниц равно 0
    page = 1;                  // Устанавливаем количество страниц равным 1

  for (k = 0; k < page; k++) // Цикл по страницам
  {
    for (j = 0; j < chWidth; j++) // Цикл по ширине символа
    {
      for (i = 0; i < 8; i++) // Цикл по высоте символа
      {
        if (pgm_read_byte(pBmp + j + k * chWidth) & (0x01 << (i & 7))) // Если бит в битовой карте равен 1
        {
          GRAY::pixel(x + j, yy + i + k * 8, 1); // Рисуем пиксель
        }
      }
    }
  }
}