#include <Arduino.h>
#include <ESP32Servo.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
// #include <WiFiManager.h>
#include <http_parser.h>
#include <this_esp_camera.h>
#include <this_esp_web_server.h>
#include <this_wheelchair.h>

EspWebServer WebServer;
EspWebServer StreamServer;
EspCamera Camera;
Servo CameraTilt;
L298WheelChair WheelChair(12, 13, 15, 14, 2, 4);

const EspWebServerHandler inputHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  EspWebServerWsFrameProcessor processor = [res](httpd_ws_frame_t *frame) {
    if (frame->len != 3) {
      return;
    }

    int8_t *payload = (int8_t *)frame->payload;

    WheelChair.move(payload[0], payload[1]);
    CameraTilt.write(payload[2] * 170 / 100);
  };

  req->receiveWsFrame(processor);
};

const EspWebServerHandler indexHtmlHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/html", SPIFFS, "/index.html");
};

const EspWebServerHandler indexCssHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/css", SPIFFS, "/index.css");
};

const EspWebServerHandler indexJsHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->send(HTTPD_200, "text/javascript", SPIFFS, "/index.js");
};

const EspWebServerHandler streamHandler = [](EspWebServerRequest *req, EspWebServerResponse *res) {
  res->setType("multipart/x-mixed-replace;boundary=123456789000000000000987654321");

  res->setHeader("Access-Control-Allow-Origin", "*");
  res->setHeader("X-Framerate", "60");

  uint8_t partBuf[128];

  EspCameraPictureReader reader = [res, partBuf](camera_fb_t *fb) {
    res->send("\r\n--123456789000000000000987654321\r\n");

    ssize_t partBufSize = snprintf((char *)partBuf, 128,
                                   "Content-Type: image/jpeg\r\nContent-Length: "
                                   "%u\r\nX-Timestamp: %d.%06d\r\n\r\n\0",
                                   fb->len, fb->timestamp.tv_sec, fb->timestamp.tv_usec);

    res->send(partBuf, partBufSize);

    res->send(fb->buf, fb->len);
  };

  while (res->isConnected()) {
    Camera.capture(reader);
  }
};

void setup() {
  Serial.begin(115200);

  Serial.println("\nStarting...");

  WiFi.softAP("rocketspy", "rocketspy");
  // WiFi.begin("", "");
  // WiFi.setSleep(false);

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  Camera.begin();

  WebServer.begin(80);

  WebServer.on("/", HTTP_GET, indexHtmlHandler);
  WebServer.on("/index.css", HTTP_GET, indexCssHandler);
  WebServer.on("/index.js", HTTP_GET, indexJsHandler);
  WebServer.on("/input", HTTP_GET, inputHandler);

  StreamServer.begin(81);

  StreamServer.on("/stream", HTTP_GET, streamHandler);

  WheelChair.begin();

  CameraTilt.attach(16);

  Serial.print("Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() { delay(150); }
