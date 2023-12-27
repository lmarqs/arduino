#include <Servo.h>

Servo servo;

int pos = 0;
const int min = 90;
const int max = 90;
void setup() { servo.attach(9); }

void loop() {
  for (pos = min; pos <= max; pos += 1) {
    servo.write(pos);
    delay(15);
  }

  for (pos = max; pos >= min; pos -= 1) {
    servo.write(pos);
    delay(15);
  }
}
