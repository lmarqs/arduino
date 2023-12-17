#include <Arduino.h>
#include <esp_camera.h>
#define CAMERA_MODEL_AI_THINKER
#include "esp_camera_pins.h"
#include "esp_camera_setup.h"


class CameraPictureReader
{
public:
  virtual void receivePicture(camera_fb_t *fb);

  virtual void readPicture();
};

class ESP32Camera
{
public:
  void begin()
  {
    esp_camera_setup();
  }

  void sendPicture(CameraPictureReader *reader)
  {
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
      Serial.println("Frame buffer could not be acquired");
      return;
    }

    reader->receivePicture(fb);

    esp_camera_fb_return(fb);

    reader->readPicture();
  }
};
