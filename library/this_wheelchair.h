#include <Arduino.h>

class WheelChair {
  virtual void move(int speedA, int speedB);
};

class L298PWheelChair : public WheelChair {
 private:
  byte pwmA;
  byte pwmB;
  byte dirA;
  byte dirB;

  void setPinModes();
  void setPinsOff();

 public:
  L298PWheelChair(byte pwmA, byte dirA, byte pwmB, byte dirB);
  void begin();
  void move(int speedA, int speedB);
};
