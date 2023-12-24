#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME "blejoystick"
#define SERVICE_UUID "2ab61dc3-ef26-4d92-aa2c-ec180a167047"
#define CHARACTERISTIC_UUID "b1af0278-69ab-4a0f-bbd0-811ce0947198"

BLECharacteristic *userInput;

class {
 public:
  uint8_t buffer[3] = {0, 0, 0};
  size_t size = 3;

  void begin() {
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
  }

  void read() {
    buffer[0] = analogRead(A0) == 0;
    buffer[1] = map(analogRead(A1), 0, 4095, -100, 100);
    buffer[2] = map(analogRead(A2), 0, 4095, -100, 100);
  }
} joystick;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

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
  userInput->setValue(joystick.buffer, joystick.size);
  userInput->notify();
  delay(50);
}
