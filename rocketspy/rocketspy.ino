#include <Arduino.h>
#include <WiFi.h>
#include <http_parser.h>
#include "camera.h"
#include "webserver.h"
#include "index.html.h"

RocketSpyWebServer WebServer;
RocketSpyCamera Camera;

const RocketSpyWebServerHandler indexHandler = [](RocketSpyRequest *req, RocketSpyResponse *res)
{
  res->send(HTTPD_200, HTTPD_TYPE_TEXT, reinterpret_cast<char *>(rocketspy_index_html), rocketspy_index_html_len);
};

const RocketSpyWebServerHandler streamHandler = [](RocketSpyRequest *req, RocketSpyResponse *res)
{
  res->setType("multipart/x-mixed-replace;boundary=123456789000000000000987654321");

  res->setHeader("Access-Control-Allow-Origin", "*");

  res->setHeader("X-Framerate", "60");

  char *part_buf[128];

  RocketSpyCameraPictureReader reader = [res, part_buf](camera_fb_t *fb)
  {
    res->reset();

    res->write("\r\n--123456789000000000000987654321\r\n", 36);

    size_t hlen = snprintf((char *)part_buf, 128, "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n", fb->len, fb->timestamp.tv_sec, fb->timestamp.tv_usec);

    res->write((const char *)part_buf, hlen);
    res->write((const char *)fb->buf, fb->len);
  };

  while (true)
  {
    Camera.capture(reader);
  }
};

void setup(void)
{
  Serial.begin(9600);

  Serial.println("\nStarting...");

  WiFi.softAP("rocketspy", "rocketspy");

  Camera.begin();

  WebServer.begin();

  WebServer.on("/", HTTP_GET, indexHandler);

  WebServer.on("/stream", HTTP_GET, streamHandler);

  Serial.println("Started");
}

void loop()
{
  delay(1000);
}
