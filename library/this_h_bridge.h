#pragma once

#include <Arduino.h>

#include "this_analog_pin.h"
#include "this_digital_pin.h"

class HBridge {
 private:
  uint8_t noSignalCount;

 protected:
  void signal();

 public:
  void noSignal(uint8_t max = 10);
  virtual void write(int32_t value);
};

class FullHBridge : public HBridge {
 private:
  OutPin *en, *i1, *i2;

 public:
  FullHBridge(OutPin* en, OutPin* i1, OutPin* i2);
  void begin();
  void write(int32_t value);
};

class HalfHBridge : public HBridge {
 private:
  OutPin *en, *i1, *i2;

 public:
  HalfHBridge(OutPin* i1, OutPin* i2);
  void begin();
  void write(int32_t value);
};
