#include <Arduino.h>

// Класс для отправки данных
template <typename T>
class DATATX
{
public:
    DATATX(T *data) : data(data) {}

    void send()
    {
        byte crc = calculateCRC((byte *)data, sizeof(T)); // Вычисляем CRC
        Serial.write((byte *)data, sizeof(T));            // Отправляем данные
        Serial.write(crc);                                // Отправляем CRC
    }

private:
    T *data;

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
    bool receive(T &data)
    {
        if (Serial.available() >= sizeof(T) + 1)
        { // +1 для CRC
            // Чтение данных
            Serial.readBytes((byte *)&data, sizeof(T));
            byte receivedCRC = Serial.read(); // Чтение CRC

            // Проверка CRC
            byte calculatedCRC = calculateCRC((byte *)&data, sizeof(T));

            // Serial.println(receivedCRC);
            // Serial.println(calculatedCRC);

            if (receivedCRC == calculatedCRC)
            {
                // Подтверждение приема
                Serial.write('A'); // Отправляем подтверждение
                return true;       // Данные приняты успешно
            }
            else
            {
                Serial.println("CRC error: Data corrupted.");
                return false; // Ошибка CRC
            }
        }
        return false; // Данные не получены
    }
};