#include "ex.h"
#include "user_xbm.h"

Graphics gfx;
Terminal trm;

TaskDispatcher dispatcher; UserIcon icons;

void f(int x, int y)
{
    int arrPin[9] = {0, 0, 35, 34, 19, 6, 7, 21, 22};

    x += 5;
    y += 40;
    int squareSize = 10;
    int spacing = 5; // Пространство между квадратами

    for (int i = 0; i < 9; i++) {
        u8g2.drawFrame(x, y, squareSize, squareSize); // Рисуем квадрат
        u8g2.setCursor(x + 2, y + 2); // Устанавливаем курсор для вывода текста
        u8g2.print(arrPin[i]); // Выводим цифру из массива

        x += squareSize + spacing; // Перемещаемся к следующему месту для квадрата

        // Если мы нарисовали 5 квадратов в первом ряду, переходим ко второму ряду
        if (i == 4) {
            x += 5;
            y += squareSize + spacing;
        }
    }

    // for(int i = 2; i < 9; i++)
    // {
    //     pinMode(arrPin[i], OUTPUT);
    //     digitalWrite(arrPin[i], 1);
    // }
}

void userTest()
{
    exForm *userForm = new exForm();

    eButton *button1 = new eButton("Test DataPort", NULL, 5, 5);
    eFunction *func1 = new eFunction([](){f(5,5);});

    userForm->title = "User form";
    userForm->eFormShowMode = FULLSCREEN;

    userForm->addElement(button1);
    userForm->addElement(func1);

    formsStack.push(userForm);
}

void setup()
{  
    gfx.initializationSystem();
    dispatcher.addTask({"User test", userTest, icons.MyComPort, DESKTOP, 0, false});
}

void loop()
{  
    dispatcher.terminal3();
}