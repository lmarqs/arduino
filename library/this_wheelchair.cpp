#include "this_wheelchair.h"

#include <Arduino.h>

void WheelChair::noSignal(uint8_t max) {
  noSignalCounter++;

  if (noSignalCounter > max) {
    move(0, 0);
  }
}

void WheelChair::signal() { noSignalCounter = 0; }

void WheelChair::begin() {
  setPinModes();

  setPinsOff();
}

FullBridgeWheelChair::FullBridgeWheelChair(byte enA, byte in1, byte in2, byte in3, byte in4, byte enB) {
  this->enA = enA;
  this->in1 = in1;
  this->in2 = in2;
  this->in3 = in3;
  this->in4 = in4;
  this->enB = enB;
}

void FullBridgeWheelChair::begin() { WheelChair::begin(); }

void FullBridgeWheelChair::setPinModes() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void FullBridgeWheelChair::setPinsOff() {
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void FullBridgeWheelChair::move(int8_t speedA, int8_t speedB) {
  WheelChair::signal();
  setSpeed(in1, in2, enA, speedA);
  setSpeed(in3, in4, enB, speedB);
}

void FullBridgeWheelChair::setSpeed(byte pin1, byte pin2, byte en, int8_t speed) {
  if (speed >= 0) {
    setSpeed(pin1, pin2, enA, map(speed, 0, 100, 0, 255));
  } else {
    setSpeed(pin2, pin1, enA, map(speed, -100, 0, 255, 0));
  }
}

void FullBridgeWheelChair::writeValues(byte high, byte low, byte en, int8_t value) {
  digitalWrite(high, HIGH);
  digitalWrite(low, LOW);
  analogWrite(en, value);
}

HalfBridgeWheelChair::HalfBridgeWheelChair(byte in1, byte in2, byte in3, byte in4) {
  this->in1 = in1;
  this->in2 = in2;
  this->in3 = in3;
  this->in4 = in4;
}

void HalfBridgeWheelChair::setPinModes() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void HalfBridgeWheelChair::setPinsOff() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void HalfBridgeWheelChair::begin() { WheelChair::begin(); }

void HalfBridgeWheelChair::move(int8_t speedA, int8_t speedB) {
  WheelChair::signal();

  if (!speedA) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  } else if (speedA > 0) {
    digitalWrite(in1, map(speedA, 0, 100, 0, 255));
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, map(speedA, -100, 0, 255, 0));
  }

  if (!speedB) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  } else if (speedB > 0) {
    digitalWrite(in3, map(speedB, 0, 100, 0, 255));
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, map(speedB, -100, 0, 255, 0));
  }
}
