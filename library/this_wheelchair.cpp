#include "this_wheelchair.h"

#include <Arduino.h>

L298WheelChair::L298WheelChair(byte pwmA, byte dirA, byte pwmB, byte dirB) {
  this->isUsing6Pins = false;
  this->enA = pwmA;
  this->in1 = in1;
  this->in3 = in3;
  this->enB = pwmB;
}

L298WheelChair::L298WheelChair(byte enA, byte in1, byte in2, byte in3, byte in4, byte enB) {
  this->isUsing6Pins = true;
  this->enA = enA;
  this->in1 = in1;
  this->in2 = in2;
  this->in3 = in3;
  this->in4 = in4;
  this->enB = enB;
}

void L298WheelChair::begin() {
  setPinModes();

  setPinsOff();
}

void L298WheelChair::setPinModes() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in3, OUTPUT);

  if (isUsing6Pins) {
    pinMode(in2, OUTPUT);
    pinMode(in4, OUTPUT);
  }
}

void L298WheelChair::setPinsOff() {
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in3, LOW);

  if (isUsing6Pins) {
    digitalWrite(in2, LOW);
    digitalWrite(in4, LOW);
  }
}

void L298WheelChair::move(int speedA, int speedB) {
  if (speedA >= 0) {
    digitalWrite(in1, HIGH);
    if (isUsing6Pins) {
      digitalWrite(in2, LOW);
    }
    analogWrite(enA, speedA * 255 / 100);
  } else {
    digitalWrite(in1, LOW);
    if (isUsing6Pins) {
      digitalWrite(in2, HIGH);
    }
    analogWrite(enA, -speedA * 255 / 100);
  }

  if (speedB >= 0) {
    digitalWrite(in3, HIGH);
    if (isUsing6Pins) {
      digitalWrite(in4, LOW);
    }
    analogWrite(enB, speedB * 255 / 100);
  } else {
    digitalWrite(in3, LOW);
    if (isUsing6Pins) {
      digitalWrite(in4, HIGH);
    }
    analogWrite(enB, -speedB * 255 / 100);
  }
}
