#include <Arduino.h>
#include <BLEDevice.h>
#include <this_wheelchair.h>

static BLEUUID serviceUUID("2ab61dc3-ef26-4d92-aa2c-ec180a167047");
static BLEUUID userInputUUID("b1af0278-69ab-4a0f-bbd0-811ce0947198");

static boolean doConnect = false;
static BLERemoteCharacteristic* userInput;
static BLEAdvertisedDevice* serverDevice;

HalfBridgeWheelChair motors(0, 1, 2, 3);

static void handleUserInput(BLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify) {
  // Serial.print("Notify callback for characteristic ");
  // Serial.print(c->getUUID().toString().c_str());
  // Serial.print(" of data length ");
  // Serial.println(length);
  // Serial.printf("data: ");
  // for (int i = 0; i < length; i++) {
  //   Serial.printf("%d ", (int8_t)data[i]);
  // }
  // Serial.println();

  motors.move(data[1], data[2]);

  if (data[0]) {
    digitalWrite(4, HIGH);
  } else {
    digitalWrite(4, LOW);
  }
}

class ClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {}

  void onDisconnect(BLEClient* pclient) { Serial.println("onDisconnect"); }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(serverDevice->getAddress().toString().c_str());

  BLEClient* client = BLEDevice::createClient();
  Serial.println(" - Created client");

  client->setClientCallbacks(new ClientCallbacks());

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

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  BLEDevice::init("");

  BLEScan* scan = BLEDevice::getScan();

  scan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);
  scan->start(5, false);

  motors.begin();

  Serial.println("Ready!");
}

void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  motors.noSignal();

  delay(100);
}
