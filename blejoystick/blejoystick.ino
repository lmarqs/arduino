#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <this_analog_pin.h>

#define DEVICE_NAME "blejoystick"
#define SERVICE_UUID "2ab61dc3-ef26-4d92-aa2c-ec180a167047"
#define CHARACTERISTIC_UUID "b1af0278-69ab-4a0f-bbd0-811ce0947198"

BLECharacteristic *userInput;
AnalogInPin x(A0);
AnalogInPin y(A1);
AnalogInPin b(A2);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  x.begin();
  y.begin();
  b.begin();

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

uint8_t inputs[] = {0, 0, 0};

void loop() {
  inputs[0] = map(x.read(), 0, 0xFFF, 0, 0xFF);
  inputs[1] = map(y.read(), 0, 0xFFF, 0, 0xFF);
  inputs[2] = map(b.read(), 0, 0xFFF, 0, 0xFF);

  userInput->setValue(inputs, 3);
  userInput->notify();
}
