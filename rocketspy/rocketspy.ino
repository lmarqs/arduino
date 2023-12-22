#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <http_parser.h>
#include <this_esp_camera.h>
#include <this_esp_web_server.h>
#include <this_wheelchair.h>

#include "index.css.h"
#include "index.html.h"
#include "index.js.h"

EspWebServer WebServer;
EspWebServer StreamServer;
EspCamera Camera;
L298PWheelChair WheelChair(12, 13, 14, 15);

const EspWebServerHandler inputHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  Serial.println("Input received");

  EspWebServerWsFrameProcessor processor = [res](httpd_ws_frame_t *frame) {
    int8_t *payload = (int8_t *)frame->payload;

    WheelChair.move(payload[0], payload[1]);
  };

  req->receiveWsFrame(processor);
};

const EspWebServerHandler indexHtmlHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/html", (char *)__index_html, __index_html_len);
};

const EspWebServerHandler indexCssHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/css", (char *)__index_css, __index_css_len);
};

const EspWebServerHandler indexJsHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/javascript", (char *)__index_js, __index_js_len);
};

const EspWebServerHandler streamHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->setType("multipart/x-mixed-replace;boundary=123456789000000000000987654321");

  res->setHeader("Access-Control-Allow-Origin", "*");
  res->setHeader("X-Framerate", "60");

  char *part_buf[128];

  EspCameraPictureReader reader = [res, part_buf](camera_fb_t *fb) {
    res->write("\r\n--123456789000000000000987654321\r\n", 36);

    size_t hlen = snprintf((char *)part_buf, 128,
                           "Content-Type: image/jpeg\r\nContent-Length: "
                           "%u\r\nX-Timestamp: %d.%06d\r\n\r\n",
                           fb->len, fb->timestamp.tv_sec, fb->timestamp.tv_usec);

    res->write((char *)part_buf, hlen);
    res->write((char *)fb->buf, fb->len);
  };

  while (res->isConnected()) {
    Camera.capture(reader);
  }
};

void setup() {
  Serial.begin(9600);

  Serial.println("\nStarting...");

  WiFi.softAP("rocketspy", "rocketspy");

  Camera.begin();

  WebServer.begin(80);

  WebServer.on("/", HTTP_GET, indexHtmlHandler);
  WebServer.on("/index.css", HTTP_GET, indexCssHandler);
  WebServer.on("/index.js", HTTP_GET, indexJsHandler);
  WebServer.on("/input", HTTP_GET, inputHandler);

  StreamServer.begin(81);

  StreamServer.on("/stream", HTTP_GET, streamHandler);

  WheelChair.begin();

  Serial.println("Started");
}

void loop() {

  delay(150);
}
