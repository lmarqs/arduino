#include "this_esp_web_server.h"

#include <Arduino.h>
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

void EspWebServerResponse::setStatus(char *status) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_status(req, status);
}

void EspWebServerResponse::setType(char *type) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_type(req, type);
}

void EspWebServerResponse::setHeader(char *key, char *value) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_hdr(req, key, value);
}

void EspWebServerResponse::write(char *buf, ssize_t len) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_send_chunk(req, buf, len);
}

void EspWebServerResponse::send(char *status, char *type, char *buf, ssize_t len) {
  this->setStatus(status);
  this->setType(type);
  err = httpd_resp_send(req, buf, len);
}

void EspWebServerResponse::sendWsFrame(httpd_ws_frame_t *frame) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_ws_send_frame(req, buf);
}

void EspWebServer::begin(int port) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.server_port = port;
  config.ctrl_port = 32000 + port;

  httpd_start(&httpd, &config);
}

void EspWebServer::on(char *uri, httpd_method_t method, EspWebServerHandler handler) {
  httpd_uri_t config;
  config.uri = uri config.method = method;

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
