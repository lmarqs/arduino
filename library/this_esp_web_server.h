#pragma once

#include <Arduino.h>
#include <FS.h>
#define CONFIG_HTTPD_WS_SUPPORT
#include <esp_http_server.h>

using EspWebServerWsFrameProcessor = std::function<void(httpd_ws_frame_t *frame)>;

class EspWebServerRequest {
 private:
  httpd_req_t *req;

 public:
  EspWebServerRequest(httpd_req_t *req);

  int getMethod();

  void receiveWsFrame(EspWebServerWsFrameProcessor processor);
};

class EspWebServerResponse {
 private:
  httpd_req_t *req;
  esp_err_t err;

 public:
  EspWebServerResponse(httpd_req_t *req);

  esp_err_t getErr();

  void reset();

  void fail();

  bool isConnected();

  void setStatus(const char *status);

  void setType(const char *type);

  void setHeader(const char *key, const char *value);

  void send(const char *status, const char *type, const uint8_t *buf, ssize_t len);

  void send(const char *status, const char *type, FS fs, const char *path);

  void send(const char *buf);

  void send(const uint8_t *buf, ssize_t len);

  void send(httpd_ws_frame_t *buf);
};

using EspWebServerHandler = std::function<void(EspWebServerRequest *req, EspWebServerResponse *res)>;

class EspWebServer {
 private:
  httpd_handle_t httpd = NULL;

 public:
  void begin(int port);

  void on(char *uri, httpd_method_t method, EspWebServerHandler handler);
};
