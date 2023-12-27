#define ROCKETSPY_CREATE_AP
#define ROCKETSPY_AP_SSID "rocketspy"
#define ROCKETSPY_AP_PASSWORD "rocketspy"

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#ifndef ROCKETSPY_CREATE_AP
#include <WiFiManager.h>
#endif
#include <Wire.h>
#include <http_parser.h>
#include <this_esp_camera.h>
#include <this_esp_web_server.h>

#include "data/index.css.h"
#include "data/index.html.h"
#include "data/index.js.h"

EspWebServer WebServer;
EspWebServer StreamServer;
EspCamera Camera;

EspWebServerFrameHandler inputFrameHandler = [](httpd_ws_frame_t *frame) {
    Wire.beginTransmission(0x01);
    Wire.write(frame->payload, frame->len);
    Wire.endTransmission();
};

const EspWebServerHandler inputHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  if (req->getMethod() == HTTP_GET) {
    return;
  }

  res->fail(req->frame(inputFrameHandler));
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

  Wire.begin(14, 15);

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
  WebServer.on("/input", HTTP_GET, inputHandler);

  StreamServer.begin(81);

  StreamServer.on("/stream.jpeg", HTTP_GET, streamHandler);

  Serial.println("Ready!");
}

void loop() {
  delay(10000);
}
