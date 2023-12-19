#include "this_wheelchair.h"

#include <Arduino.h>

L298PWheelChair::L298PWheelChair(byte pwmA, byte dirA, byte pwmB, byte dirB) {
  this->pwmA = pwmA;
  this->dirA = dirA;
  this->pwmB = pwmB, this->dirB = dirB;
}

void L298PWheelChair::begin() {
  setPinModes();

  setPinsOff();
}

void L298PWheelChair::setPinModes() {
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmA, OUTPUT);
}

void L298PWheelChair::setPinsOff() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  digitalWrite(pwmA, LOW);
  digitalWrite(pwmB, LOW);
}

void L298PWheelChair::move(int speedA, int speedB) {
  if (speedA >= 0) {
    digitalWrite(dirA, HIGH);
    analogWrite(pwmA, speedA * 255 / 100);
  } else {
    digitalWrite(dirA, LOW);
    analogWrite(pwmA, -speedA * 255 / 100);
  }

  if (speedB >= 0) {
    digitalWrite(dirB, HIGH);
    analogWrite(pwmB, speedB * 255 / 100);
  } else {
    digitalWrite(dirB, LOW);
    analogWrite(pwmB, -speedB * 255 / 100);
  }
}
