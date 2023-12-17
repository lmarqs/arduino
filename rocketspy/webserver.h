#include <Arduino.h>
#include <esp_http_server.h>

class RocketSpyRequest
{
private:
  httpd_req_t *req;

public:
  RocketSpyRequest(httpd_req_t *req)
  {
    this->req = req;
  }
};

class RocketSpyResponse
{
private:
  httpd_req_t *req;
  esp_err_t err;

public:
  RocketSpyResponse(httpd_req_t *req)
  {
    this->req = req;
    this->err = ESP_OK;
  }

  esp_err_t getErr()
  {
    return err;
  }

  void reset()
  {
    err = ESP_OK;
  }

  void fail()
  {
    err = ESP_FAIL;
  }

  bool isConnected()
  {
    return err == ESP_OK;
  }

  void setStatus(const char *status)
  {
    if (err != ESP_OK)
    {
      return;
    }

    err = httpd_resp_set_status(req, status);
  }

  void setType(const char *type)
  {
    if (err != ESP_OK)
    {
      return;
    }

    err = httpd_resp_set_type(req, type);
  }

  void setHeader(const char *key, const char *value)
  {
    if (err != ESP_OK)
    {
      return;
    }

    err = httpd_resp_set_hdr(req, key, value);
  }

  void write(const char *buf, ssize_t len)
  {
    if (err != ESP_OK)
    {
      return;
    }

    err = httpd_resp_send_chunk(req, buf, len);
  }

  void send(const char *status, const char *type, const char *buf, ssize_t len)
  {
    this->setStatus(status);
    this->setType(type);
    err = httpd_resp_send(req, buf, len);
  }
};

using RocketSpyWebServerHandler = std::function<void(RocketSpyRequest *req, RocketSpyResponse *res)>;

class RocketSpyWebServer
{
private:
  httpd_handle_t httpd = NULL;

public:
  void begin(int port)
  {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.server_port = port;
    config.ctrl_port = 32000 + port;

    httpd_start(&httpd, &config);
  }

  void on(const char *uri, const httpd_method_t method, const RocketSpyWebServerHandler handler)
  {
    httpd_uri_t config = {
        .uri = uri,
        .method = method,
        .handler = [](httpd_req_t *r) -> esp_err_t
        {
          RocketSpyRequest req(r);
          RocketSpyResponse res(r);

          RocketSpyWebServerHandler *handler = static_cast<RocketSpyWebServerHandler *>(r->user_ctx);

          (*handler)(&req, &res);

          return res.getErr();
        },
        .user_ctx = new RocketSpyWebServerHandler(handler),
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL};

    httpd_register_uri_handler(httpd, &config);
  }
};
