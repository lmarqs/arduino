#include <Arduino.h>
#include <this_esp_ledc_pin.h>
#include <this_h_bridge.h>

EspLedcOutPin en1(32, 0, 5000., 8);
DigitalOutPin in11(33);
DigitalOutPin in12(25);

FullHBridge motor1(&en1, &in11, &in12);

void setup() {
  motor1.begin();
  // rearMotors.begin();
}

void loop() {
  motor1.write(0xFF);

  delay(1000);

  motor1.write(0x00);

  delay(1000);

  motor1.write(-0xFF);

  delay(1000);

  motor1.write(0x00);

  delay(1000);
}
