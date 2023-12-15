#include <Arduino.h>

void setup()
{
  Serial.begin(9600);
  pinMode(18, OUTPUT);
}

void loop()
{
  Serial.println("HIGH");
  digitalWrite(18, HIGH);
  delay(1000);
  Serial.println("LOW");
  digitalWrite(18, LOW);
  delay(1000);
}
