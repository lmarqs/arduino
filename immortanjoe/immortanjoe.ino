#include <RoboCore_Vespa.h>
#include <Arduino.h>
#include <Bluepad32.h>

VespaMotors motors;
Controller *controller = nullptr;
uint8_t speed = 0;
uint8_t speedL = 0;
uint8_t speedR = 0;
const uint8_t MOTOR_MIN_SPEED = 50;
const uint8_t MOTOR_MAX_SPEED = 100;
const int32_t CONTROLLER_AXIS_DEATH_VALUE = 128;
const int32_t CONTROLLER_AXIS_MAX_VALUE = 512;

int32_t hardcapControllerAxisValue(int32_t axisValue)
{
  if (axisValue > CONTROLLER_AXIS_MAX_VALUE)
  {
    return CONTROLLER_AXIS_MAX_VALUE;
  }

  if (axisValue < -CONTROLLER_AXIS_MAX_VALUE)
  {
    return -CONTROLLER_AXIS_MAX_VALUE;
  }

  return axisValue;
}

int32_t ignoreControllerAxisDeathZone(int32_t axisValue)
{
  if (axisValue < CONTROLLER_AXIS_DEATH_VALUE && axisValue > -CONTROLLER_AXIS_DEATH_VALUE)
  {
    return 0;
  }

  return axisValue;
}

void onConnectedController(Controller *c)
{
  if (controller == nullptr)
  {
    controller = c;
  }
}

void onDisconnectedController(Controller *c)
{
  if (controller == c)
  {
    controller = nullptr;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
}

void loop()
{
  BP32.update();

  speed = 0;
  speedL = 0;
  speedR = 0;

  if (controller != nullptr)
  {
    int32_t y = ignoreControllerAxisDeathZone(hardcapControllerAxisValue(controller->axisRY()));
    int32_t x = hardcapControllerAxisValue(controller->axisX());

    speed = -y * 100 / CONTROLLER_AXIS_MAX_VALUE;

    if (speed > MOTOR_MIN_SPEED)
    {
      speedL = speedR = speed;

      if (x > 0)
      {
        speedR = speedR - x * 100 / CONTROLLER_AXIS_MAX_VALUE;
      }

      if (x < 0)
      {
        speedL = speedL + x * 100 / CONTROLLER_AXIS_MAX_VALUE;
      }

      Serial.printf("%d\t%d\t%d\t%d\t%d\n", y, x, speed, speedL, speedR);
      motors.turn(speedL, speedR);
    }

    if (speed < -MOTOR_MIN_SPEED)
    {
      speedL = speedR = speed;

      if (x > 0)
      {
        speedL -= x * 100 / CONTROLLER_AXIS_MAX_VALUE;
      }

      if (x < 0)
      {
        speedR += x * 100 / CONTROLLER_AXIS_MAX_VALUE;
      }

      Serial.printf("%d\t%d\t%d\t%d\t%d\n", y, x, speed, speedL, speedR);
      motors.turn(speedL, speedR);
    }
  }

  if (!speed)
  {
    motors.stop();
  }

  vTaskDelay(1);
  delay(150);
}
