#define WEBSPY_CREATE_AP
#define WEBSPY_AP_SSID "rocketspy"
#define WEBSPY_AP_PASSWORD "rocketspy"

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <http_parser.h>
#include <this_esp_camera.h>
#include <this_esp_ledc_pin.h>
#include <this_esp_web_server.h>

#include "data/index.css.h"
#include "data/index.html.h"
#include "data/index.js.h"

EspWebServer WebServer;
EspWebServer StreamServer;
EspCamera Camera;
EspLedcOutPin Spotlight(4, 2, 5000., 10);
EspLedcOutPin Tilt(12, 3, 50., 10);

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

EspWebServerBodyHandler spotlightBodyHandler = [](uint8_t *buf, size_t len) {
  uint16_t data = 0;

  if (sizeof(data) != len) {
    return;
  }

  memcpy(&data, buf, len);

  Spotlight.write(data);
};

const EspWebServerHandler spotlightHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->fail(req->body(spotlightBodyHandler));
  res->setHeader("Access-Control-Allow-Origin", "*");
  res->send(HTTPD_200, "text/plain", NULL, 0);
};

EspWebServerBodyHandler tiltBodyHandler = [](uint8_t *buf, size_t len) {
  uint16_t data = 0;

  if (sizeof(data) != len) {
    return;
  }

  memcpy(&data, buf, len);

  uint32_t value = map(data, 1023, 0, 58, 116);

  Tilt.write(value);
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
  res->setType("multipart/x-mixed-replace;boundary=espcameraframe");
  res->setHeader("Access-Control-Allow-Origin", "*");
  res->setHeader("X-Framerate", "60");

  uint8_t buf[128];

  EspCameraPictureReader reader = [res, buf](camera_fb_t *fb) {
    res->send((uint8_t *)"\r\n--espcameraframe\r\n", 36);

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
  Serial.begin(9600);

  Serial.println("\nStarting...");

  Wire.begin(14, 15);

  Camera.begin();

  Spotlight.begin();

  Tilt.begin();

  WiFiManager wifiManager;

  wifiManager.resetSettings();

#ifdef WEBSPY_CREATE_AP
  Serial.println("Creating AP.");

  WiFi.softAP(WEBSPY_AP_SSID, WEBSPY_AP_PASSWORD);

  Serial.println("AP created!");

  Serial.print("IP: ");

  Serial.println(WiFi.softAPIP());
#else
  Serial.println("Connecting to WiFi.");

  WiFi.mode(WIFI_STA);

  WiFi.begin(WEBSPY_AP_SSID, WEBSPY_AP_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\n");

  Serial.println("Connected to the WiFi network!");

  Serial.print("IP: ");

  Serial.println(WiFi.localIP());
#endif
  WiFi.printDiag(Serial);

  WebServer.begin(80);

  WebServer.on("/", HTTP_GET, indexHtmlHandler);
  WebServer.on("/index.css", HTTP_GET, indexCssHandler);
  WebServer.on("/index.js", HTTP_GET, indexJsHandler);
  WebServer.on("/input", HTTP_GET, inputHandler);
  WebServer.on("/tilt", HTTP_POST, tiltHandler);
  WebServer.on("/spotlight", HTTP_POST, spotlightHandler);

  StreamServer.begin(81);

  StreamServer.on("/stream.jpeg", HTTP_GET, streamHandler);

  Serial.println("Ready!");
}

void loop() { delay(10000); }
