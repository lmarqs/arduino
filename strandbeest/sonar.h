#include <Arduino.h>
#include <this_digital_pin.h>

DigitalInPin echo(10);
DigitalOutPin trigger(12);

class Sonar {
 public:
  void begin() {
    echo.begin();
    trigger.begin();
  }

  float read() {
    trigger.write(LOW);
    delayMicroseconds(5);
    trigger.write(HIGH);
    delayMicroseconds(10);
    trigger.write(LOW);

    uint64_t duration = pulseIn(10, HIGH);

    return duration * 0.017;
  }
};

Sonar sonar;
