#include <Arduino.h>

class WheelChair {
  virtual void move(int speedA, int speedB);
};

class L298WheelChair : public WheelChair {
 private:
  byte enA, in1, in2, in3, in4, enB;
  boolean isUsing6Pins;

  void setPinModes();
  void setPinsOff();

 public:
  L298WheelChair(byte pwmA, byte dirA, byte pwmB, byte dirB);
  L298WheelChair(byte enA, byte in1, byte in2, byte in3, byte in4, byte enB);
  void begin();
  void move(int speedA, int speedB);
};
