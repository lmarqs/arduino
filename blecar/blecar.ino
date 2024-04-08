#include <Arduino.h>
#include <BLEDevice.h>
#include <this_analog_pin.h>
#include <this_digital_pin.h>
#include <this_h_bridge.h>

DigitalOutPin led(8);

AnalogOutPin in1(1);
AnalogOutPin in2(2);
AnalogOutPin in3(4);
AnalogOutPin in4(3);

#define SPEED_MAX_VALUE 0x7F

HalfHBridge motor1(&in1, &in2);
HalfHBridge motor2(&in3, &in4);

BLEUUID serviceUUID("2ab61dc3-ef26-4d92-aa2c-ec180a167047");
BLEUUID userInputUUID("b1af0278-69ab-4a0f-bbd0-811ce0947198");

boolean doConnect = false;
boolean connected = false;

BLERemoteCharacteristic* userInput;
BLEAdvertisedDevice* serverDevice;

int32_t ignoreControllerAxisDeathZone(int32_t axisValue, int32_t min, int32_t max) {
  if (axisValue < min || axisValue > max) {
    return axisValue;
  }

  return 0x800;
}

void handleUserInput(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify) {
  uint32_t* input = (uint32_t*)data;

  // Serial.printf("input[0]:\t%d, input[1]:\t%d, input[2]:\t%d\n", input[0], input[1], input[2]);

  int axisX = map(ignoreControllerAxisDeathZone(input[0], 1600, 2200), 0, 0xFFF, -SPEED_MAX_VALUE, SPEED_MAX_VALUE);
  int axisY = map(ignoreControllerAxisDeathZone(input[1], 1500, 2100), 0, 0xFFF, -SPEED_MAX_VALUE, SPEED_MAX_VALUE);
  // Serial.printf("axisX:\t%d, axisY:\t%d\n", axisX, axisY);

  int8_t speed1 = constrain(axisY + min(axisX, 0), -SPEED_MAX_VALUE, SPEED_MAX_VALUE);
  int8_t speed2 = constrain(axisY - max(axisX, 0), -SPEED_MAX_VALUE, SPEED_MAX_VALUE);
  // Serial.printf("speed1:\t%d, speed2:\t%d\n", speed1, speed2);

  motor1.move(speed1);
  motor2.move(speed2);

  delay(50);
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

void blink(uint8_t times = 3, uint8_t delayMs = 150) {
  while (times > 0) {
    times--;
    led.write(HIGH);
    delay(delayMs);
    led.write(LOW);
    delay(delayMs);
  }
}

void setup() {
  Serial.begin(9600);

  Serial.println("Starting...");

  led.begin();

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

  if (!connected) {
    blink(3);
    delay(250);
  }

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
