#include <esp_camera.h>
#define CAMERA_MODEL_AI_THINKER
#include "esp_camera_pins.h"
#include "esp_camera_setup.h"

using RocketSpyCameraPictureReader = std::function<void(camera_fb_t *)>;

class RocketSpyCamera
{
private:
  sensor_t *sensor;

public:
  void begin()
  {
    sensor = esp_camera_setup();

    if (sensor)
    {
      sensor->set_framesize(sensor, FRAMESIZE_VGA);
    }
  }

  void capture(RocketSpyCameraPictureReader reader)
  {
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
      Serial.println("Frame buffer could not be acquired");
      return;
    }

    reader(fb);

    esp_camera_fb_return(fb);
  }
};
