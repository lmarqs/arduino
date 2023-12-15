#include <Arduino.h>
#include <IRremote.hpp>

int const MAX_SPEED = 80;
uint8_t const PIN_IN1 = 5;
uint8_t const PIN_IN2 = 6;

void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(3);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
}

void loop()
{
  if (IrReceiver.decode())
  {
    if (IrReceiver.decodedIRData.command == 0x18)
    {
      analogWrite(PIN_IN1, MAX_SPEED);
      analogWrite(PIN_IN2, 0);
    }

    if (IrReceiver.decodedIRData.command == 0x52)
    {
      analogWrite(PIN_IN1, 0);
      analogWrite(PIN_IN2, MAX_SPEED);
    }

    IrReceiver.resume();
  }
  else
  {
    analogWrite(PIN_IN1, 0);
    analogWrite(PIN_IN2, 0);
  }

  delay(150);
}
