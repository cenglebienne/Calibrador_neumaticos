#include "http_server_app.h"

WebServerType Miwebserver(80);

void webserver_begin() {
    Miwebserver.begin();
}

void webserver_loop() {
    Miwebserver.handleClient();
}
