# arduino

## Dependencies
- make
- gcc
- arduino-cli
- python 3.10

## Setup

```
make init
```

## Packages

### immortanjoe
WhellChair controlled by gamepad

```
make FBQN=esp32-bluepad32:esp32:esp32 PORT=/dev/ttyUSB0 SKETCH=immortanjoe run
```

### tonta
Simple motor mini car controlled by IR

```
make FBQN=stm8:stm8:stm8 PORT=/dev/ttyUSB0 SKETCH=tonta run
```
