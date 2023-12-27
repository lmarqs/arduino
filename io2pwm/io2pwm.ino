#include <SoftRcPulseOut.h>

#define INPUT_PIN 2
#define SERVO_PIN 3

SoftRcPulseOut servo;

void setup() {
  pinMode(INPUT_PIN, INPUT);
  servo.attach(SERVO_PIN);
}

int input = 0;

void loop() {
  input = analogRead(INPUT_PIN);

  servo.write(map(input, 0, 1023, 0, 180));
}
