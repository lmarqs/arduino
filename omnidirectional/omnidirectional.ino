#include <Arduino.h>
#include <Bluepad32.h>
#include <this_digital_pin.h>
#include <this_esp_ledc_pin.h>
#include <this_h_bridge.h>
#include <this_pin.h>

EspLedcOutPin pwm1(13, 1, 5000, 12);
DigitalOutPin in11(12);
DigitalOutPin in21(14);
FullHBridge motor1(&pwm1, &in11, &in21);

EspLedcOutPin pwm2(25, 2, 5000, 12);
DigitalOutPin in12(26);
DigitalOutPin in22(27);
FullHBridge motor2(&pwm2, &in12, &in22);

EspLedcOutPin pwm3(22, 3, 5000, 12);
DigitalOutPin in13(21);
DigitalOutPin in23(19);
FullHBridge motor3(&pwm3, &in13, &in23);

EspLedcOutPin pwm4(17, 4, 5000, 12);
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

int32_t *rotation = new int32_t[motors_len];
int32_t *translation = new int32_t[motors_len];

Controller *controller = NULL;

int32_t ignoreControllerAxisDeathZone(int32_t axisValue) {
  int32_t deathZone = 62;

  if (axisValue < deathZone && axisValue > -deathZone) {
    return 0;
  }

  return axisValue;
}

int32_t hardcapControllerAxisValue(int32_t axisValue) {
  int32_t maxValue = 512;

  if (axisValue > maxValue) {
    return maxValue;
  }

  if (axisValue < -maxValue) {
    return -maxValue;
  }

  return axisValue;
}

uint32_t applySpeedCurve(int32_t speed, int32_t max) { return speed * speed / max + 2 * speed; }

void translate(int32_t axisY, int32_t axisX, int32_t normal, int32_t speeds[]) {
  if (axisY <= 0 && axisX >= 0) {
    speeds[0] = speeds[2] = -axisY - axisX;
    speeds[1] = speeds[3] = +normal;
  }

  if (axisY >= 0 && axisX >= 0) {
    speeds[0] = speeds[2] = -normal;
    speeds[1] = speeds[3] = -axisY + axisX;
  }

  if (axisY >= 0 && axisX <= 0) {
    speeds[0] = speeds[2] = -axisY - axisX;
    speeds[1] = speeds[3] = -normal;
  }

  if (axisY <= 0 && axisX <= 0) {
    speeds[0] = speeds[2] = +normal;
    speeds[1] = speeds[3] = -axisY + axisX;
  }
}

void rotate(int32_t axisY, int32_t axisX, int32_t normal, int32_t *speeds) {
  if (axisY <= 0 && axisX >= 0) {
    speeds[0] = speeds[3] = +normal;
    speeds[1] = speeds[2] = -axisY - axisX;
  }

  if (axisY >= 0 && axisX >= 0) {
    speeds[0] = speeds[3] = -axisY + axisX;
    speeds[1] = speeds[2] = -normal;
  }

  if (axisY >= 0 && axisX <= 0) {
    speeds[0] = speeds[3] = -normal;
    speeds[1] = speeds[2] = -axisY - axisX;
  }

  if (axisY <= 0 && axisX <= 0) {
    speeds[0] = speeds[3] = -axisY + axisX;
    speeds[1] = speeds[2] = +normal;
  }
}

void setup() {
  Serial.begin(115200);

  BP32.setup([controller](Controller *c) { controller = controller == NULL ? c : controller; },
             [controller](Controller *c) { controller = controller == c ? NULL : controller; });

  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  for (size_t i = 0; i < motors_len; i++) {
    motors[i].begin();
  }
}

void loop() {
  BP32.update();

  if (!controller) {
    motors->stop();
    return;
  }

  int32_t axisLY = hardcapControllerAxisValue(ignoreControllerAxisDeathZone(controller->axisY()));
  int32_t axisLX = hardcapControllerAxisValue(ignoreControllerAxisDeathZone(controller->axisX()));
  int32_t axisRY = hardcapControllerAxisValue(ignoreControllerAxisDeathZone(controller->axisRY()));
  int32_t axisRX = hardcapControllerAxisValue(ignoreControllerAxisDeathZone(controller->axisRX()));

  int32_t normalL = hardcapControllerAxisValue(sqrt((axisLY * axisLY) + (axisLX * axisLX)));
  int32_t normalR = hardcapControllerAxisValue(sqrt((axisRY * axisRY) + (axisRX * axisRX)));

  for (size_t i = 0; i < motors_len; i++) {
    rotation[i] = 0;
    translation[i] = 0;
  }

  rotate(axisLY, axisLX, normalL, rotation);
  translate(axisRY, axisRX, normalR, translation);

  for (size_t i = 0; i < motors_len; i++) {
    int32_t speed = normalL + normalR ? (rotation[i] * normalL + translation[i] * normalR) / (normalL + normalR) : 0;

    if (speed >= 0) {
      uint32_t normal = map(+speed, 0, 512, 0, 0b111111111111 * 0.80);
      motors[i].foward(normal);
      // Serial.printf("s%d:\t%d\t%u\t%d\n", i, speed, normal, normalL);
    } else {
      uint32_t normal = map(-speed, 0, 512, 0, 0b111111111111 * 0.80);
      motors[i].backward(normal);
      // Serial.printf("s%d:\t%d\t%u\t%d\n", i, speed, normal, normalL);
    }
  }
}
