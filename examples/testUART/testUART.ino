#include "rxtx.h"

struct Data {
  short a, b;
  bool s;
};

Data receivedData;
short aa; short bb; short ss;

DATARX receiver;

void setup() {
  Serial.begin(9600);  // Инициализация Serial
}

void loop() {

  if (Serial.available() > 0) {
    // Прием данных
    if (receiver.receive(receivedData)) {
      // Данные успешно приняты
      Serial.print("a: ");
      Serial.println(receivedData.a);
      Serial.print("b: ");
      Serial.println(receivedData.b);
      Serial.print("s: ");
      Serial.println(receivedData.s);

      aa = receivedData.a;
      bb = receivedData.b;
      ss = receivedData.s;
    }
  }

  Serial.print(aa); Serial.print(bb); Serial.println(ss);

  delay(1000);
}
