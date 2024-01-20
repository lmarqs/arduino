#include <Arduino.h>
#include <this_digital_pin.h>
#include <this_esp_ledc_pin.h>
#include <this_h_bridge.h>
#include <this_pin.h>

EspLedcOutPin pwm1(13, 1, 5000., 8);
DigitalOutPin in11(12);
DigitalOutPin in21(14);
FullHBridge motor1(&pwm1, &in11, &in21);

EspLedcOutPin pwm2(25, 2, 5000., 8);
DigitalOutPin in12(26);
DigitalOutPin in22(27);
FullHBridge motor2(&pwm2, &in12, &in22);

EspLedcOutPin pwm3(22, 3, 5000., 8);
DigitalOutPin in13(21);
DigitalOutPin in23(19);
FullHBridge motor3(&pwm3, &in13, &in23);

EspLedcOutPin pwm4(17, 4, 5000., 8);
DigitalOutPin in14(5);
DigitalOutPin in24(18);
FullHBridge motor4(&pwm4, &in14, &in24);

FullHBridge motors[] = {
    motor1,
    motor2,
    motor3,
    motor4,
};

size_t motors_len = sizeof(motors) / sizeof(FullHBridge);

void testMotor(HBridge* motor) {
  for (uint32_t i = 0xA0; i <= 0xFF; i++) {
    motor->foward(i);
    Serial.println(i, HEX);
    delay(100);
  }

  delay(1000);

  motor->stop();

  delay(1000);

  for (uint32_t i = 0xA0; i <= 0xFF; i++) {
    motor->backward(i);
    Serial.println(i, HEX);
    delay(100);
  }

  delay(1000);

  motor->stop();

  delay(1000);
}

void setup() {
  Serial.begin(115200);

  for (size_t i = 0; i < motors_len; i++) {
    motors[i].begin();
  }
}

void loop() {
}
