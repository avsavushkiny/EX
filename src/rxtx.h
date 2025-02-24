/*
    DataPort RX, TX
    [02/2025, Alexander Savushkin]
*/

#include <Arduino.h>
#pragma once;


//1-GND, 2-3v3, 3-GPIO35, 4-GPIO34, 5-GPIO19, 6-TX, 7-RX, 8-GPIO21, 9-GPIO22
/* TX */
class DataTransmitter
{
public:
    // Конструктор, принимающий данные для передачи
    DataTransmitter(const void *data, size_t dataSize) : data(data), dataSize(dataSize)
    {
        Serial.begin(9600);
    }

    // Метод для отправки данных
    bool sendData()
    {
        byte crc = calculateCRC((byte *)data, dataSize); // Вычисляем CRC
        int retries = 3;                                 // Количество попыток передачи

        while (retries > 0)
        {
            // Отправка данных
            Serial.write((byte *)data, dataSize);
            Serial.write(crc); // Отправляем CRC

            // Ожидание подтверждения
            if (waitForAck())
            {
                Serial.println("Data sent successfully.");
                return true;
            }
            else
            {
                Serial.println("Timeout: No acknowledgment received. Retrying...");
                retries--;
            }
        }

        Serial.println("Transmission failed after retries.");
        return false;
    }

private:
    const void *data; // Указатель на данные для передачи
    size_t dataSize;  // Размер данных

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

    // Метод для ожидания подтверждения с тайм-аутом
    bool waitForAck(unsigned long timeout = 1000)
    {
        unsigned long startTime = millis();
        while (!Serial.available())
        {
            if (millis() - startTime > timeout)
            { // Тайм-аут
                return false;
            }
        }
        char ack = Serial.read();
        return (ack == 'A'); // Возвращает true, если получено подтверждение
    }

    /*
        MyData data;
        data.id = 123;
        data.value = 45.67;
        data.status = true;

        // Создание объекта DataTransmitter с данными
        DataTransmitter transmitter(&data, sizeof(data));

        // Отправка данных
        if (transmitter.sendData()) {
            Serial.println("Data transmission successful!");
        } else {
            Serial.println("Data transmission failed.");
        }
    */
};
/* RX */
class DataReceiver
{
public:
    // Конструктор
    DataReceiver()
    {
        Serial.begin(9600);
    }

    // Метод для приема данных
    template <typename T>
    bool receiveData(T &data)
    {
        if (Serial.available() >= sizeof(data) + 1)
        { // +1 для CRC
            // Чтение данных
            Serial.readBytes((byte *)&data, sizeof(data));
            byte receivedCRC = Serial.read(); // Чтение CRC

            // Проверка CRC
            byte calculatedCRC = calculateCRC((byte *)&data, sizeof(data));
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

    /*
    DataReceiver receiver;
    MyData receivedData;

    // Прием данных
    if (receiver.receiveData(receivedData)) {
        // Данные успешно приняты
        Serial.print("ID: ");
        Serial.println(receivedData.id);
        Serial.print("Value: ");
        Serial.println(receivedData.value);
        Serial.print("Status: ");
        Serial.println(receivedData.status);
    } else {
        // Ошибка приема данных
        Serial.println("Failed to receive data.");
    }

    delay(1000); // Задержка для удобства
    */
};

class DataTX
{
public:
    DataTX(const void *data, size_t dataSize) : data(data), dataSize(dataSize)
    {
        Serial.begin(9600);
    }
    
    void sendData()
    {
        Serial.write((byte *)data, dataSize);
    }

private:
    const void *data; // Указатель на данные для передачи
    size_t dataSize;  // Размер данных
};

class DataRX
{
public:
    DataRX(void *data, size_t dataSize) : data(data), dataSize(dataSize)
    {
        Serial.begin(9600);
    }

    void receiveData()
    {
        if (Serial.available() >= sizeof(data))
        {
            Serial.readBytes((byte *)data, sizeof(data));
        }
    }

private:
    void *data; // Указатель на данные для передачи
    size_t dataSize;  // Размер данных
};