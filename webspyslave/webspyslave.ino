#include <RoboCore_Vespa.h>
#include <Wire.h>

VespaMotors *motors;

void receiveEvent(int len) {
  size_t size = 2;

  if (len != size) {
    return;
  }

  int8_t buf[size];

  int8_t i = 0;

  while (Wire.available()) {
    buf[i++] = Wire.read();
  }

  if (i != size) {
    return;
  }

  motors->turn(buf[0], buf[1]);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(0x01);
  Wire.onReceive(receiveEvent);

  motors = new VespaMotors();
}

void loop() {
  delay(10000);
}
