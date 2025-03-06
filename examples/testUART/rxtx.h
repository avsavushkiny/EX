#include <Arduino.h>

// Класс для отправки данных
// template <typename T>
class DATATX
{
public:
    // DATATX(T *data) : data(data) {}
    DATATX() {}

    template <typename T>
    void sendData(T &data, uint8_t numberPort)
    {
        byte crc = calculateCRC((byte *)&data, sizeof(T)); // Вычисляем CRC

        if (numberPort == 0)
        {
            Serial.write((byte *)&data, sizeof(T)); // Отправляем данные
            Serial.write(crc);                     // Отправляем CRC
        }
        if (numberPort == 1)
        {
            Serial1.write((byte *)&data, sizeof(T)); // Отправляем данные
            Serial1.write(crc);                     // Отправляем CRC
        }
    }

private:
    // T *data;

    // Метод для вычисления контрольной суммы (CRC8)
    byte calculateCRC(const byte *data, size_t length)
    {
        byte crc = 0;
        for (size_t i = 0; i < length; i++)
        {
            crc ^= data[i];
            for (byte j = 0; j < 8; j++)
            {
                if (crc & 0x80)
                {
                    crc = (crc << 1) ^ 0x07;
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }
};
// Класс для приема данных
class DATARX
{
private:
    // Метод для вычисления контрольной суммы (CRC8)
    byte calculateCRC(const byte *data, size_t length)
    {
        byte crc = 0;
        for (size_t i = 0; i < length; i++)
        {
            crc ^= data[i];
            for (byte j = 0; j < 8; j++)
            {
                if (crc & 0x80)
                {
                    crc = (crc << 1) ^ 0x07;
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

public:
    // Конструктор
    DATARX()
    {
        // Инициализация Serial должна быть вынесена в setup()
    }

    // Метод для приема данных
    template <typename T>
    bool receive(T &data, int8_t numberPort)
    {
        if (numberPort == 0)
        {
            if (Serial.available() >= sizeof(data) + 1)
            { // +1 для CRC
                // Чтение данных
                Serial.readBytes((byte *)&data, sizeof(T));
                byte receivedCRC = Serial.read(); // Чтение CRC

                // Проверка CRC
                byte calculatedCRC = calculateCRC((byte *)&data, sizeof(T));

                // Serial.print(receivedCRC, HEX);
                // Serial.print(" CRC ");
                // Serial.println(calculatedCRC, HEX);

                if (receivedCRC == calculatedCRC)
                {
                    // Подтверждение приема
                    // Serial.write('A'); // Отправляем подтверждение
                    return true;       // Данные приняты успешно
                }
                else
                {
                    // Serial.println("CRC error: Data corrupted.");
                    return false; // Ошибка CRC
                }
            }
            return false; // Данные не получены
        }

        if (numberPort == 1)
        {
            if (Serial1.available() >= sizeof(data) + 1)
            { // +1 для CRC
                // Чтение данных
                Serial1.readBytes((byte *)&data, sizeof(T));
                byte receivedCRC = Serial1.read(); // Чтение CRC

                // Проверка CRC
                byte calculatedCRC = calculateCRC((byte *)&data, sizeof(T));

                // Serial.print(receivedCRC, HEX); Serial.print(" CRC ");
                // Serial.println(calculatedCRC, HEX);

                if (receivedCRC == calculatedCRC)
                {
                    // Подтверждение приема
                    // Serial1.write('A'); // Отправляем подтверждение
                    return true; // Данные приняты успешно
                }
                else
                {
                    // Serial.println("CRC error: Data corrupted.");
                    return false; // Ошибка CRC
                }
            }
            return false; // Данные не получены
        }

        // if (Serial.available() >= sizeof(data) + 1)
        // { // +1 для CRC
        //     // Чтение данных
        //     Serial.readBytes((byte *)&data, sizeof(T));
        //     byte receivedCRC = Serial.read(); // Чтение CRC

        //     // Проверка CRC
        //     byte calculatedCRC = calculateCRC((byte *)&data, sizeof(T));

        //     Serial.print(receivedCRC, HEX); Serial.print(" CRC ");
        //     Serial.println(calculatedCRC, HEX);

        //     if (receivedCRC == calculatedCRC)
        //     {
        //         // Подтверждение приема
        //         Serial.write('A'); // Отправляем подтверждение
        //         return true;       // Данные приняты успешно
        //     }
        //     else
        //     {
        //         Serial.println("CRC error: Data corrupted.");
        //         return false; // Ошибка CRC
        //     }
        // }
        // return false; // Данные не получены
    }
};