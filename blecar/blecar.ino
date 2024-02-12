#include <Arduino.h>
#include <BLEDevice.h>
#include <this_esp_ledc_pin.h>
#include <this_h_bridge.h>

DigitalOutPin led(4);

EspLedcOutPin in1(5, 1, 5000, 12);
EspLedcOutPin in2(6, 2, 5000, 12);
EspLedcOutPin in3(8, 3, 5000, 12);
EspLedcOutPin in4(7, 4, 5000, 12);

HalfHBridge motor1(&in1, &in2);
HalfHBridge motor2(&in3, &in4);

BLEUUID serviceUUID("2ab61dc3-ef26-4d92-aa2c-ec180a167047");
BLEUUID userInputUUID("b1af0278-69ab-4a0f-bbd0-811ce0947198");

boolean doConnect = false;
boolean connected = false;

BLERemoteCharacteristic* userInput;
BLEAdvertisedDevice* serverDevice;

void handleUserInput(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify) {
  int8_t axisX = map(data[0], 0, 0xFF, -128, 127);
  int8_t axisY = map(data[1], 0, 0xFF, -128, 127);
  bool button = map(data[2], 0, 0xFF, true, false);

  int8_t normal = constrain(sqrt((axisX * axisX) + (axisY * axisY)), 0, 127);

  int8_t speed1 = 0;
  int8_t speed2 = 0;

  if (axisX >= 0 && axisY <= 0) {
    speed1 = -axisY - axisX;
    speed2 = +normal;
  }

  if (axisX >= 0 && axisY >= 0) {
    speed1 = -normal;
    speed2 = -axisY + axisX;
  }

  if (axisX <= 0 && axisY >= 0) {
    speed1 = -axisY - axisX;
    speed2 = -normal;
  }

  if (axisX <= 0 && axisY <= 0) {
    speed1 = +normal;
    speed2 = -axisY + axisX;
  }

  int32_t maxSpeed = 127;
  int32_t maxMotorsValue = 0b110011001100;

  if (speed1 > 0) {
    motor1.backward(map(speed1, 0, maxSpeed, 0, maxMotorsValue));
  } else {
    motor1.forward(map(-speed1, 0, maxSpeed, 0, maxMotorsValue));
  }

  if (speed2 > 0) {
    motor2.backward(map(speed2, 0, maxSpeed, 0, maxMotorsValue));
  } else {
    motor2.forward(map(-speed2, 0, maxSpeed, 0, maxMotorsValue));
  }
}

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(serverDevice->getAddress().toString().c_str());

  BLEClient* client = BLEDevice::createClient();
  Serial.println(" - Created client");

  client->connect(serverDevice);

  Serial.println(" - Connected to server");

  client->setMTU(517);

  BLERemoteService* remoteService = client->getService(serviceUUID);

  if (remoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    client->disconnect();
    return false;
  }

  Serial.println(" - Found our service");

  userInput = remoteService->getCharacteristic(userInputUUID);

  if (userInput == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(userInputUUID.toString().c_str());
    client->disconnect();
    return false;
  }

  Serial.println(" - Found our characteristic");

  userInput->registerForNotify(handleUserInput);

  return true;
}

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      serverDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

void setup() {
  Serial.begin(115200);

  Serial.println("Starting...");

  motor1.begin();
  motor2.begin();

  BLEDevice::init("");

  BLEScan* scan = BLEDevice::getScan();

  scan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);
  scan->start(5, false);

  Serial.printf("\nReady!\n");
}

void loop() {
  motor1.noSignal();
  motor2.noSignal();

  // if (!connected) {
  //   led.write(LOW);
  //   delay(500);
  //   led.write(HIGH);
  //   delay(500);
  // }

  if (doConnect) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  delay(100);
}
