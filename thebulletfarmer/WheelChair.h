#include <Arduino.h>

class WheelChair
{
  virtual void accelerate(byte speed);
  virtual void goFoward();
  virtual void goBackward();
  virtual void turnLeft();
  virtual void turnRight();
};

class L298PWheelChair : public WheelChair
{
public:
  L298PWheelChair(byte pwmA,
                  byte pwmB,
                  byte dirA,
                  byte dirB);
  void begin();
  void accelerate(byte speed);
  void goFoward();
  void goBackward();
  void turnLeft();
  void turnRight();

private:
  byte pwmA;
  byte pwmB;
  byte dirA;
  byte dirB;
  // byte currentSpeed = 0;

  void setPinModes();
  void setPinsOff();
};

L298PWheelChair::L298PWheelChair(
    byte pwmA,
    byte pwmB,
    byte dirA,
    byte dirB)
{
  this->pwmA = pwmA;
  this->pwmB = pwmB,
  this->dirA = dirA;
  this->dirB = dirB;
}

void L298PWheelChair::begin()
{
  setPinModes();

  setPinsOff();
}

void L298PWheelChair::setPinModes()
{
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmA, OUTPUT);
}

void L298PWheelChair::setPinsOff()
{
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  digitalWrite(pwmA, LOW);
  digitalWrite(pwmB, LOW);
}

void L298PWheelChair::goFoward()
{
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
}

void L298PWheelChair::goBackward()
{
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
}

void L298PWheelChair::turnLeft()
{
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);
}

void L298PWheelChair::turnRight()
{
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);
}

void L298PWheelChair::accelerate(byte speed)
{
  analogWrite(pwmA, speed);
  analogWrite(pwmB, speed);
  // while (speed != currentSpeed)
  // {
  //   currentSpeed += speed > currentSpeed ? 1 : -1;

  //   analogWrite(pwmA, currentSpeed);
  //   analogWrite(pwmB, currentSpeed);
  // }
}

class L298NWheelChair : public WheelChair
{
public:
  L298NWheelChair(byte pinIn1,
                  byte pinIn2,
                  byte pinIn3,
                  byte pinIn4);
  void begin();
  void accelerate(byte speed);
  void goFoward();
  void goBackward();
  void turnLeft();
  void turnRight();

private:
  byte pinIn1;
  byte pinIn2;
  byte pinIn3;
  byte pinIn4;
  byte pinEna;
  byte pinEnb;

  void setPinModes();
  void setPinsOff();
};

L298NWheelChair::L298NWheelChair(
    byte pinIn1,
    byte pinIn2,
    byte pinIn3,
    byte pinIn4)
{
  this->pinIn1 = pinIn1;
  this->pinIn2 = pinIn2,
  this->pinIn3 = pinIn3;
  this->pinIn4 = pinIn4;
  this->pinEna = pinIn1;
  this->pinEnb = pinIn3;
}

void L298NWheelChair::begin()
{
  setPinModes();

  setPinsOff();
}

void L298NWheelChair::setPinModes()
{
  pinMode(pinIn1, OUTPUT);
  pinMode(pinIn2, OUTPUT);
  pinMode(pinIn3, OUTPUT);
  pinMode(pinIn4, OUTPUT);
}

void L298NWheelChair::setPinsOff()
{
  digitalWrite(pinIn1, LOW);
  digitalWrite(pinIn2, LOW);
  digitalWrite(pinIn3, LOW);
  digitalWrite(pinIn4, LOW);
}

void L298NWheelChair::goFoward()
{
  pinEna = pinIn1;
  pinEnb = pinIn3;
  setPinsOff();
}

void L298NWheelChair::goBackward()
{
  pinEna = pinIn2;
  pinEnb = pinIn4;
  setPinsOff();
}

void L298NWheelChair::turnLeft()
{
  pinEna = pinIn1;
  pinEnb = pinIn4;
  setPinsOff();
}

void L298NWheelChair::turnRight()
{
  pinEna = pinIn2;
  pinEnb = pinIn3;
  setPinsOff();
}

void L298NWheelChair::accelerate(byte speed)
{
  analogWrite(pinEna, speed);
  analogWrite(pinEnb, speed);
}
