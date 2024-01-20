#include "this_h_bridge.h"

#include <Arduino.h>

void HBridge::noSignal(uint8_t max) {
  if (++noSignalCount > max) {
    return;
  }

  write(0);
}

void HBridge::signal() { noSignalCount = 0; }

FullHBridge::FullHBridge(OutPin* en, OutPin* i1, OutPin* i2) {
  this->en = en;
  this->i1 = i1;
  this->i2 = i2;
}

void FullHBridge::begin() {
  en->begin();
  i1->begin();
  i2->begin();
}

void FullHBridge::write(int32_t value) {
  HBridge::signal();

  if (value == 0) {
    i1->write(LOW);
    i2->write(LOW);
    en->write(0);
  } else if (value > 0) {
    i1->write(HIGH);
    i2->write(LOW);
    en->write(value);
  } else {
    i1->write(LOW);
    i2->write(HIGH);
    en->write(-value);
  }

  en->write(value);
}

HalfHBridge::HalfHBridge(OutPin* i1, OutPin* i2) {
  this->i1 = i1;
  this->i2 = i2;
}

void HalfHBridge::begin() {
  i1->begin();
  i2->begin();
}

void HalfHBridge::write(int32_t value) {
  HBridge::signal();

  if (value == 0) {
    i1->write(LOW);
    i2->write(LOW);
  } else if (value > 0) {
    i1->write(value);
    i2->write(LOW);
  } else {
    i1->write(LOW);
    i2->write(-value);
  }
}
