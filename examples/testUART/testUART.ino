#include "rxtx.h"

struct Data {
  short a, b;
  char t[20];
  bool s;
  bool d;
};

Data receivedData;

DATARX receiver;

void setup() {
  Serial1.begin(9600);  // Инициализация Serial
}

void loop() {

  if (Serial1.available() > 0) {
    // Прием данных
    receiver.receive(receivedData, 1);
    // Данные успешно приняты
    Serial.println(receivedData.a);
    Serial.println(receivedData.b);
    Serial.println(receivedData.t);
    Serial.println(receivedData.s);
    Serial.println(receivedData.d);
  }
}
