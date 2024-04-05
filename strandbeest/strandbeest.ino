#include <Arduino.h>

#include <IRremote.hpp>

#include "controller.h"
#include "legs.h"
#include "sonar.h"

void setup() {
  Serial.begin(9600);

  sonar.begin();

  right.begin();
  left.begin();

  IrReceiver.begin(2);
}

void loop() {
  left.noSignal();
  right.noSignal();

  if (IrReceiver.decode()) {
    IrReceiver.resume();

    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      IrReceiver.printIRResultShort(&Serial);

      if (IrReceiver.decodedIRData.command == CONTROLLER_COMMAND_UP) {
        left.forward(255);
        right.forward(255);
      }

      if (IrReceiver.decodedIRData.command == CONTROLLER_COMMAND_DOWN) {
        left.backward(255);
        right.backward(255);
      }

      if (IrReceiver.decodedIRData.command == CONTROLLER_COMMAND_LEFT) {
        left.forward(255);
        right.backward(255);
      }

      if (IrReceiver.decodedIRData.command == CONTROLLER_COMMAND_RIGHT) {
        left.backward(255);
        right.forward(255);
      }
    }

    delay(100);
  }

  // Serial.print(sonar.read());
  // Serial.print("cm");
  // Serial.println();

  // delay(250);
}
