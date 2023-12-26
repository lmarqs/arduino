/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010, LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * Arduino srl - www.arduino.org
 * Base on lib for stm32f4 (d2a4a47):
 * https://github.com/arduino-libraries/Servo/blob/master/src/stm32f4/ServoTimers.h
 * 2017 Jul 5: Edited by Jaroslav Páral (jarekparal) - paral@robotikabrno.cz
 */


#include "this_esp_servo.h"

#include <Arduino.h>
#include <esp32-hal-ledc.h>

EspServo::EspServo() { resetFields(); }

EspServo::~EspServo() { detach(); }

bool EspServo::attach(int pin, int channel, int minAngle, int maxAngle, int minPulseWidthUs, int maxPulseWidthUs,
                   int frequency) {
  int periodUs = std::round(1000000.0 / frequency);

  if (periodUs <= maxPulseWidthUs) {
    return false;
  }

  this->pin = pin;
  this->channel = channel;
  this->minAngle = minAngle;
  this->maxAngle = maxAngle;
  this->minPulseWidthUs = minPulseWidthUs;
  this->maxPulseWidthUs = maxPulseWidthUs;
  this->periodUs = periodUs;

  ledcSetup(channel, frequency, TIMER_RESOLUTION);

  ledcAttachPin(pin, this->channel);

  return true;
}

bool EspServo::detach() {
  if (!attached()) {
    return false;
  }

  ledcDetachPin(pin);

  pin = PIN_NOT_ATTACHED;

  return true;
}

void EspServo::write(int angle) {
  angle = constrain(angle, minAngle, maxAngle);

  writeMicroseconds(angleToUs(angle));
}

void EspServo::writeMicroseconds(int pulseWidthUs) {
  if (!attached()) {
    return;
  }

  pulseWidthTicks = usToTicks(constrain(pulseWidthUs, minPulseWidthUs, maxPulseWidthUs));

  ledcWrite(channel, pulseWidthTicks);
}

int EspServo::read() { return usToAngle(readMicroseconds()); }

int EspServo::readMicroseconds() {
  if (!attached()) {
    return 0;
  }

  int duty = ledcRead(channel);

  return ticksToUs(duty);
}

bool EspServo::attached() { return pin != PIN_NOT_ATTACHED; }

int EspServo::attachedPin() { return pin; }

void EspServo::resetFields(void) {
  pin = PIN_NOT_ATTACHED;
  pulseWidthTicks = 0;
  channel = CHANNEL_NOT_ATTACHED;
  minAngle = DEFAULT_MIN_ANGLE;
  maxAngle = DEFAULT_MAX_ANGLE;
  minPulseWidthUs = DEFAULT_MIN_PULSE_WIDTH_US;
  maxPulseWidthUs = DEFAULT_MAX_PULSE_WIDTH_US;
  periodUs = 1000000 / DEFAULT_FREQUENCY;
}

int EspServo::usToTicks(int us) { return std::round((PERIOD_TICKS * us) / periodUs); }
int EspServo::ticksToUs(int duty) { return std::round((periodUs * duty) / PERIOD_TICKS); }
int EspServo::usToAngle(int us) { return map(us, minPulseWidthUs, maxPulseWidthUs, minAngle, maxAngle); }
int EspServo::angleToUs(int angle) { return map(angle, minAngle, maxAngle, minPulseWidthUs, maxPulseWidthUs); }
