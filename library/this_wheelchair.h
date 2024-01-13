#pragma once

#include <Arduino.h>

class WheelChair {
 private:
  uint8_t noSignalCounter;
  virtual void setPinModes();
  virtual void setPinsOff();

 protected:
  void signal();
  void begin();

 public:
  void noSignal(uint8_t max = 10);
  virtual void move(int8_t speedA, int8_t speedB);
};

class FullBridgeWheelChair : public WheelChair {
 private:
  byte enA, in1, in2, in3, in4, enB;
  void writeValues(byte high, byte low, byte en, int8_t value);
  void setSpeed(byte pin1, byte pin2, byte en, int8_t speed);
  void setPinModes();
  void setPinsOff();

 public:
  FullBridgeWheelChair(byte enA, byte in1, byte in2, byte in3, byte in4, byte enB);
  void begin();
  void move(int8_t speedA, int8_t speedB);
};


class HalfBridgeWheelChair : public WheelChair {
 private:
  byte in1, in2, in3, in4;
  void setPinModes();
  void setPinsOff();

 public:
  HalfBridgeWheelChair(byte in1, byte in2, byte in3, byte in4);
  void begin();
  void move(int8_t speedA, int8_t speedB);
};
