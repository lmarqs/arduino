#include "this_esp_ledc.h"

#include <Arduino.h>
#include <esp32-hal-ledc.h>

EspLedc::EspLedc(uint8_t pin, uint8_t channel, uint32_t frequency, uint8_t resolution) {
  this->pin = pin;
  this->channel = channel;
  this->frequency = frequency;
  this->resolution = resolution;
}

void EspLedc::begin() {
  pinMode(pin, OUTPUT);

  ledcSetup(channel, frequency, resolution);

  ledcAttachPin(pin, channel);
}

void EspLedc::write(uint32_t duty) { ledcWrite(channel, duty); }

uint32_t EspLedc::read() { return ledcRead(channel); }
