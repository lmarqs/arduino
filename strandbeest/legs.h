#include <Arduino.h>
#include <this_analog_pin.h>
#include <this_h_bridge.h>

AnalogOutPin in1(9);
AnalogOutPin in2(6);
HalfHBridge left(&in1, &in2);

AnalogOutPin in3(5);
AnalogOutPin in4(3);
HalfHBridge right(&in3, &in4);
