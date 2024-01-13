#include <Arduino.h>

const int static pin = 8;

void setup()
{
  Serial.begin(115200);
  pinMode(pin, OUTPUT);
}

void loop()
{
  Serial.println("HIGH");
  digitalWrite(pin, HIGH);
  delay(1000);
  Serial.println("LOW");
  digitalWrite(pin, LOW);
  delay(1000);
}
