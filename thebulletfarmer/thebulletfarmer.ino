#include <Arduino.h>

#include <this_wheelchair.h>

L298WheelChair Car(25, 23, 19, 18);

void setup()
{
  Serial.begin(9600);
  Car.begin();
}

void loop()
{
  int const speed = 120;
  Car.goFoward();
  Car.accelerate(speed);
  delay(1000);

  Car.accelerate(0);
  delay(1000);

  Car.goBackward();
  Car.accelerate(speed);
  delay(1000);

  Car.accelerate(0);
  delay(1000);

  Car.turnLeft();
  Car.accelerate(speed);
  delay(1000);

  Car.accelerate(0);
  delay(1000);

  Car.turnRight();
  Car.accelerate(speed);
  delay(1000);
}
