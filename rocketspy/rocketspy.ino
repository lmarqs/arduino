#include <Arduino.h>
#include <WiFi.h>
#include <http_parser.h>
#include "camera.h"
#include "webserver.h"
#include "wheelchair.h"
#include "index.html.h"
#include "index.js.h"
#include "index.css.h"

RocketSpyWebServer WebServer;
RocketSpyWebServer StreamServer;
RocketSpyCamera Camera;
L298PWheelChair WheelChair(12, 13, 14, 15);

const RocketSpyWebServerHandler indexHtmlHandler = [](RocketSpyRequest *req, RocketSpyResponse *res)
{
  res->send(HTTPD_200, "text/html", (const char *)__index_html, __index_html_len);
};

const RocketSpyWebServerHandler indexCssHandler = [](RocketSpyRequest *req, RocketSpyResponse *res)
{
  res->send(HTTPD_200, "text/css", (const char *)__index_css, __index_css_len);
};

const RocketSpyWebServerHandler indexJsHandler = [](RocketSpyRequest *req, RocketSpyResponse *res)
{
  res->send(HTTPD_200, "text/javascript", (const char *)__index_js, __index_js_len);
};

const RocketSpyWebServerHandler streamHandler = [](RocketSpyRequest *req, RocketSpyResponse *res)
{
  res->setType("multipart/x-mixed-replace;boundary=123456789000000000000987654321");

  res->setHeader("Access-Control-Allow-Origin", "*");
  res->setHeader("X-Framerate", "60");

  char *part_buf[128];

  RocketSpyCameraPictureReader reader = [res, part_buf](camera_fb_t *fb)
  {
    res->write("\r\n--123456789000000000000987654321\r\n", 36);

    size_t hlen = snprintf((char *)part_buf, 128, "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n", fb->len, fb->timestamp.tv_sec, fb->timestamp.tv_usec);

    res->write((const char *)part_buf, hlen);
    res->write((const char *)fb->buf, fb->len);
  };

  while (res->isConnected())
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

  WebServer.begin(80);

  WebServer.on("/", HTTP_GET, indexHtmlHandler);
  WebServer.on("/index.css", HTTP_GET, indexCssHandler);
  WebServer.on("/index.js", HTTP_GET, indexJsHandler);

  StreamServer.begin(81);

  StreamServer.on("/stream", HTTP_GET, streamHandler);

  WheelChair.begin();

  Serial.println("Started");
}

void loop()
{
  for (int i = 0; i <= 100; i += 10)
  {
    WheelChair.move(i, 0);
    delay(100);
  }

  delay(1000);

  for (int i = 0; i >= -100; i += -10)
  {
    WheelChair.move(i, 0);
    delay(100);
  }

  delay(1000);

  for (int i = 0; i <= 100; i += 10)
  {
    WheelChair.move(0, i);
    delay(100);
  }

  delay(1000);

  for (int i = 0; i >= -100; i += -10)
  {
    WheelChair.move(0, i);
    delay(100);
  }

  delay(1000);
}
