#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME "blejoystick"
#define SERVICE_UUID "2ab61dc3-ef26-4d92-aa2c-ec180a167047"
#define CHARACTERISTIC_UUID "b1af0278-69ab-4a0f-bbd0-811ce0947198"

BLECharacteristic *userInput;

int16_t ignoreDeathZone(int16_t value, int16_t threshold) {
  if (value > -threshold && value < threshold) {
    return 0;
  }

  return value;
}

class {
 private:
  bool button = false;
  int16_t x = 0;
  int16_t y = 0;

 public:
  bool getButton() { return button; }
  int16_t getX() { return x; }
  int16_t getY() { return y; }

  void begin() {
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
  }

  void read() {
    button = analogRead(A0) == 0;
    x = map(analogRead(A1), 0, 4095, -2048, 2047);
    y = map(analogRead(A2), 0, 4095, -2048, 2047);
  }
} joystick;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  joystick.begin();

  BLEDevice::init(DEVICE_NAME);
  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);
  userInput = service->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                                                                     BLECharacteristic::PROPERTY_WRITE |
                                                                     BLECharacteristic::PROPERTY_NOTIFY);

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);

  BLEDevice::startAdvertising();

  Serial.println("Ready!");
}

void loop() {
  joystick.read();

  bool button = joystick.getButton();
  int16_t x = ignoreDeathZone(joystick.getX(), 520);
  int16_t y = ignoreDeathZone(joystick.getY(), 700);

  int8_t speedL = map((2048 + min((int16_t)0, x)) / 2048 * y, -2048, 2047, -100, 100);
  int8_t speedR = map((2048 - max((int16_t)0, x)) / 2048 * y, -2048, 2047, -100, 100);

  Serial.printf("%d\t%d\t%d\t%d\t%d\n", button, x, y, speedL, speedR);

  uint8_t data[] = {button, speedL, speedR};

  userInput->setValue(data, 3);
  userInput->notify();
  delay(50);
}
