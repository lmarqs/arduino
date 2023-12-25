#include "this_esp_web_server.h"

#include <Arduino.h>
#include <FS.h>
#include <esp_http_server.h>

EspWebServerRequest::EspWebServerRequest(httpd_req_t *req) { this->req = req; }

int EspWebServerRequest::getMethod() { return req->method; }

void EspWebServerRequest::receiveWsFrame(EspWebServerWsFrameProcessor processor) {
  httpd_ws_frame_t *frame = (httpd_ws_frame_t *)malloc(sizeof(httpd_ws_frame_t));

  if (!frame) {
    return;
  }

  memset(frame, 0, sizeof(httpd_ws_frame_t));

  frame->type = HTTPD_WS_TYPE_TEXT;

  esp_err_t ret = httpd_ws_recv_frame(req, frame, 0);

  if (ret != ESP_OK || !frame->len) {
    return;
  }

  frame->payload = (uint8_t *)malloc(frame->len + 1);

  if (!frame->payload) {
    return;
  }

  ret = httpd_ws_recv_frame(req, frame, frame->len);

  if (ret == ESP_OK) {
    processor(frame);
  }

  free(frame->payload);

  free(frame);
}

EspWebServerResponse::EspWebServerResponse(httpd_req_t *req) {
  this->req = req;
  this->err = ESP_OK;
}

esp_err_t EspWebServerResponse::getErr() { return err; }

void EspWebServerResponse::reset() { err = ESP_OK; }

void EspWebServerResponse::fail() { err = ESP_FAIL; }

bool EspWebServerResponse::isConnected() { return err == ESP_OK; }

void EspWebServerResponse::setStatus(const char *status) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_status(req, status);
}

void EspWebServerResponse::setType(const char *type) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_type(req, type);
}

void EspWebServerResponse::setHeader(const char *key, const char *value) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_hdr(req, key, value);
}

void EspWebServerResponse::send(const char *status, const char *type, const uint8_t *buf, size_t len) {
  this->setStatus(status);

  this->setType(type);

  err = httpd_resp_send(req, (char *)buf, len);
}

void EspWebServerResponse::send(const char *status, const char *type, FS fs, const char *path) {
  this->setStatus(status);

  this->setType(type);

  File file = fs.open(path, "r");

  req->content_len = file.size();

  uint8_t buf[128];

  size_t len;

  while (file.available() && err == ESP_OK) {
    len = file.read(buf, 128);

    this->send(buf, len);
  }

  this->send(NULL, 0);

  file.close();
}

void EspWebServerResponse::send(const uint8_t *buf, size_t len) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_send_chunk(req, (char *)buf, len);
}

void EspWebServerResponse::send(const char *buf) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_send_chunk(req, buf, strlen(buf));
}

void EspWebServerResponse::send(httpd_ws_frame_t *frame) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_ws_send_frame(req, frame);
}

void EspWebServer::begin(int port) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.server_port = port;
  config.ctrl_port = 32000 + port;

  httpd_start(&httpd, &config);
}

void EspWebServer::on(char *uri, httpd_method_t method, EspWebServerHandler handler) {
  httpd_uri_t config;

  config.uri = uri;
  config.method = method;

  config.handler = [](httpd_req_t *r) -> esp_err_t {
    EspWebServerRequest req(r);
    EspWebServerResponse res(r);

    EspWebServerHandler *handler = static_cast<EspWebServerHandler *>(r->user_ctx);

    (*handler)(&req, &res);

    return res.getErr();
  };

  config.user_ctx = new EspWebServerHandler(handler);

  config.is_websocket = true;

  config.handle_ws_control_frames = false;

  config.supported_subprotocol = NULL;

  httpd_register_uri_handler(httpd, &config);
}
