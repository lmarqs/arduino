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
#pragma once

#include <Arduino.h>
#include <esp32-hal-ledc.h>

class EspServo {
 public:

  /**
   * Default min/max pulse widths (in microseconds) and angles
   * (in degrees).  Values chosen for Arduino compatibility.
   * These values  are part of the public API; DO NOT CHANGE THEM.
   */
  static const int DEFAULT_MIN_ANGLE = 0;
  static const int DEFAULT_MAX_ANGLE = 180;

  static const int DEFAULT_MIN_PULSE_WIDTH_US = 544;   // the shortest pulse sent to a servo
  static const int DEFAULT_MAX_PULSE_WIDTH_US = 2400;  // the longest pulse sent to a servo

  static const int DEFAULT_FREQUENCY = 50;

  static const int TIMER_RESOLUTION = (16 < SOC_LEDC_TIMER_BIT_WIDE_NUM) ? 16 : SOC_LEDC_TIMER_BIT_WIDE_NUM;
  static const int PERIOD_TICKS = (1 << TIMER_RESOLUTION) - 1;

  static const int CHANNEL_NOT_ATTACHED = -1;

  // Pin number of unattached pins
  static const int PIN_NOT_ATTACHED = -1;

  /**
   * @brief Construct a new EspServo instance.
   *
   * The new instance will not be attached to any pin.
   */
  EspServo();

  /**
   * @brief Destruct a EspServo instance.
   *
   * Call _() and detach().
   */
  ~EspServo();

  /**
   * @brief Associate this instance with a servomotor whose input is
   *        connected to pin.
   * @param pin Pin connected to the servo pulse width input. This
   *            pin must be capable of PWM output (all ESP32 pins).
   *
   * @param channel Channel which is set to ESP32 Arduino function ledcSetup().
   *                Channel must be number between 0 - 15.
   *                It is possible to use automatic channel setup with constant
   *                EspServo::CHANNEL_NOT_ATTACHED.
   *
   * @param minAngle Target angle (in degrees or radians) associated with
   *                 minPulseWidthUs.  Defaults to DEFAULT_MIN_ANGLE = 0.
   *
   * @param maxAngle Target angle (in degrees or radians) associated with
   *                 maxPulseWidthUs.  Defaults to DEFAULT_MAX_ANGLE = 180.
   *
   * @param minPulseWidthUs Minimum pulse width to write to pin, in
   *                        microseconds.  This will be associated
   *                        with a minAngle angle.  Defaults to
   *                        DEFAULT_MIN_PULSE_WIDTH_US = 544.
   *
   * @param maxPulseWidthUs Maximum pulse width to write to pin, in
   *                        microseconds.  This will be associated
   *                        with a maxAngle angle. Defaults to
   *                        DEFAULT_MAX_PULSE_WIDTH_US = 2400.
   *
   * @param frequency Frequency in hz to send PWM at.
   *                  Defaults to DEFAULT_FREQUENCY.
   *
   * @sideeffect May set pinMode(pin, PWM).
   *
   * @return true if successful, false when pin doesn't support PWM.
   */
  bool attach(int pin, int channel, int minAngle = DEFAULT_MIN_ANGLE,
              int maxAngle = DEFAULT_MAX_ANGLE, int minPulseWidthUs = DEFAULT_MIN_PULSE_WIDTH_US,
              int maxPulseWidthUs = DEFAULT_MAX_PULSE_WIDTH_US, int frequency = DEFAULT_FREQUENCY);

  /**
   * @brief Stop driving the servo pulse train.
   *
   * If not currently attached to a motor, this function has no effect.
   *
   * @return true if this call did anything, false otherwise.
   */
  bool detach();

  /**
   * @brief Set the servomotor target angle.
   *
   * @param angle Target angle, in degrees or radians.  If the target
   *              angle is outside the range specified at attach() time, it
   *              will be clamped to lie in that range.
   *
   * @see EspServo::attach()
   */
  void write(int angle);

  /**
   * @brief Set the pulse width, in microseconds.
   *
   * @param pulseWidthUs Pulse width to send to the servomotor, in
   *                     microseconds. If outside of the range
   *                     specified at attach() time, it is clamped to
   *                     lie in that range.
   *
   * @see EspServoTemplate::attach()
   */
  void writeMicroseconds(int pulseWidthUs);

  /**
   * Get the servomotor's target angle, in degrees or radians.  This will
   * lie inside the range specified at attach() time.
   *
   * @see EspServo::attach()
   */
  int read();

  /**
   * Get the current pulse width, in microseconds.  This will
   * lie within the range specified at attach() time.
   *
   * @see EspServo::attach()
   */
  int readMicroseconds();

  /**
   * @brief Check if this instance is attached to a servo.
   * @return true if this instance is attached to a servo, false otherwise.
   * @see EspServo::attachedPin()
   */
  bool attached();

  /**
   * @brief Get the pin this instance is attached to.
   * @return Pin number if currently attached to a pin, PIN_NOT_ATTACHED
   *         otherwise.
   * @see EspServo::attach()
   */
  int attachedPin();

 private:
  int pin;
  int pulseWidthTicks;
  int channel;
  int minPulseWidthUs, maxPulseWidthUs;
  int minAngle, maxAngle;
  int periodUs;

  void resetFields(void);
  int usToTicks(int us);
  int ticksToUs(int duty);
  int usToAngle(int us);
  int angleToUs(int angle);
};
