#pragma once

#include <Arduino.h>

class EspLedc {
 private:
  uint8_t pin;
  uint8_t channel;
  uint32_t frequency;
  uint8_t resolution;

 public:
  EspLedc(uint8_t pin, uint8_t channel, uint32_t frequency, uint8_t resolution);

  void begin();

  uint32_t read();

  void write(uint32_t duty);
};
