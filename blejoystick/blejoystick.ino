#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME "blejoystick"
#define SERVICE_UUID "2ab61dc3-ef26-4d92-aa2c-ec180a167047"
#define CHARACTERISTIC_UUID "b1af0278-69ab-4a0f-bbd0-811ce0947198"

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

int16_t ignoreDeathZone(int16_t value, int16_t threshold) {
  if (value > -threshold && value < threshold) {
    return 0;
  }

  return value;
}

BLECharacteristic *userInput;

size_t input_size = sizeof(uint16_t);
uint8_t *inputs;
size_t inputs_len = 3 * input_size;

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

  inputs = (uint8_t *)calloc(1, inputs_len);

  Serial.println("Ready!");
}

void loop() {
  joystick.read();

  inputs[0 * input_size] = joystick.getButton();
  inputs[1 * input_size] = ignoreDeathZone(joystick.getX(), 520);
  inputs[2 * input_size] = ignoreDeathZone(joystick.getY(), 700);

  userInput->setValue(inputs, inputs_len);
  userInput->notify();
  delay(50);
}
