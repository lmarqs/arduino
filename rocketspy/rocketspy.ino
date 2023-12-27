#define ROCKETSPY_CREATE_AP
#define ROCKETSPY_AP_SSID "rocketspy"
#define ROCKETSPY_AP_PASSWORD "rocketspy"

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#ifndef ROCKETSPY_CREATE_AP
#include <WiFiManager.h>
#endif
#include <http_parser.h>
#include <this_esp_camera.h>
#include <this_esp_servo.h>
#include <this_esp_web_server.h>
#include <this_wheelchair.h>

#include "data/index.css.h"
#include "data/index.html.h"
#include "data/index.js.h"

EspWebServer WebServer;
EspWebServer StreamServer;
EspCamera Camera;
EspServo Tilt;

L298WheelChair WheelChair(12, 13, 15, 14, 2, 4);

EspWebServerFrameHandler moveFrameHandler = [](httpd_ws_frame_t *frame) {
  if (frame->len != 2) {
    return;
  }

  int8_t *payload = (int8_t *)frame->payload;

  WheelChair.move(payload[0], payload[1]);
};

const EspWebServerHandler moveHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  if (req->getMethod() == HTTP_GET) {
    return;
  }

  res->fail(req->frame(moveFrameHandler));
};

EspWebServerBodyHandler tiltBodyHandler = [](uint8_t *buf, size_t len) {
  if (len != 1) {
    return;
  }

  int8_t *payload = (int8_t *)buf;

  int angle = map(payload[0], 0, 100, 170, 0);

  Tilt.write(angle);
};

const EspWebServerHandler tiltHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->fail(req->body(tiltBodyHandler));
  res->setHeader("Access-Control-Allow-Origin", "*");
  res->send(HTTPD_200, "text/plain", NULL, 0);
};

const EspWebServerHandler indexHtmlHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/html", data_index_html, data_index_html_len);
};

const EspWebServerHandler indexCssHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/css", data_index_css, data_index_css_len);
};

const EspWebServerHandler indexJsHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/javascript", data_index_js, data_index_js_len);
};

const EspWebServerHandler streamHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->setType("multipart/x-mixed-replace;boundary=123456789000000000000987654321");
  res->setHeader("Access-Control-Allow-Origin", "*");
  res->setHeader("X-Framerate", "60");

  uint8_t buf[128];

  EspCameraPictureReader reader = [res, buf](camera_fb_t *fb) {
    res->send((uint8_t *)"\r\n--123456789000000000000987654321\r\n", 36);

    size_t len = snprintf((char *)buf, 128,
                          "Content-Type: image/jpeg\r\n"
                          "Content-Length: %u\r\n"
                          "X-Timestamp: %d.%06d\r\n"
                          "\r\n",
                          fb->len, fb->timestamp.tv_sec, fb->timestamp.tv_usec);

    res->send(buf, len);

    res->send(fb->buf, fb->len);
  };

  while (res->isConnected()) {
    Camera.capture(reader);
  }
};

void setup() {
  Serial.begin(115200);

  Serial.println("\nStarting...");

  Tilt.attach(16, 10);

  Camera.begin();

#ifdef ROCKETSPY_CREATE_AP
  WiFi.softAP(ROCKETSPY_AP_SSID, ROCKETSPY_AP_PASSWORD);
#else
  WiFiManager wifiManager;

  if (!wifiManager.autoConnect(ROCKETSPY_AP_SSID, ROCKETSPY_AP_PASSWORD)) {
    ESP.restart();
    delay(5000);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.println(WiFi.localIP());
#endif

  WebServer.begin(80);

  WebServer.on("/", HTTP_GET, indexHtmlHandler);
  WebServer.on("/index.css", HTTP_GET, indexCssHandler);
  WebServer.on("/index.js", HTTP_GET, indexJsHandler);
  WebServer.on("/move", HTTP_GET, moveHandler);
  WebServer.on("/tilt", HTTP_POST, tiltHandler);

  StreamServer.begin(81);

  StreamServer.on("/stream.jpeg", HTTP_GET, streamHandler);

  WheelChair.begin();

  Serial.println("Ready!");
}

void loop() { delay(10000); }
