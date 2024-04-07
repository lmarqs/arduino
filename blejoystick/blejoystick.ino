#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <this_analog_pin.h>
#include <this_digital_pin.h>

#define DEVICE_NAME "blejoystick"
#define SERVICE_UUID "2ab61dc3-ef26-4d92-aa2c-ec180a167047"
#define CHARACTERISTIC_UUID "b1af0278-69ab-4a0f-bbd0-811ce0947198"

DigitalOutPin led(8);

BLECharacteristic *userInput;
AnalogInPin x(A0);
AnalogInPin y(A1);
AnalogInPin b(A2);

uint32_t inputs[] = {0, 0, 0};

boolean connected = false;

void blink(uint8_t times = 3, uint8_t delayMs = 150) {
  while (times > 0) {
    times--;
    led.write(HIGH);
    delay(delayMs);
    led.write(LOW);
    delay(delayMs);
  }
}

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) {
    Serial.println("Connected");
    connected = true;
  }

  void onDisconnect(BLEServer *server) {
    Serial.println("Disconnected");
    connected = false;
  }
};

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  led.begin();

  x.begin();
  y.begin();
  b.begin();

  BLEDevice::init(DEVICE_NAME);
  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);
  userInput = service->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                                                                     BLECharacteristic::PROPERTY_WRITE |
                                                                     BLECharacteristic::PROPERTY_NOTIFY);

  server->setCallbacks(new ServerCallbacks());

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);

  BLEDevice::startAdvertising();

  Serial.println("Ready!");
}

void loop() {
  if (!connected) {
    blink(3);
  }

  inputs[0] = x.read();
  inputs[1] = y.read();
  inputs[2] = b.read();

  Serial.printf("x: %u, y: %u, b: %u\n", inputs[0], inputs[1], inputs[2]);

  userInput->setValue((uint8_t*)inputs, 3 * sizeof(uint32_t));
  userInput->notify();

  delay(100);
}
