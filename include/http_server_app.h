// include/http_server_app.h
#pragma once
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <WebServer.h>
  using WebServerType = WebServer;
#else
  #include <ESP8266WebServer.h>
  using WebServerType = ESP8266WebServer;
#endif

extern WebServerType Miwebserver;

void webserver_begin();
void webserver_loop();
