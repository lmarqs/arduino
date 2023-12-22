#include <Servo.h>

Servo myservo;

int pos = 0;
const int min = 90;
const int max = 90;
void setup() { myservo.attach(9); }

void loop() {
  for (pos = min; pos <= max; pos += 1) {
    myservo.write(pos);
    delay(15);
  }

  for (pos = max; pos >= min; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }
}
