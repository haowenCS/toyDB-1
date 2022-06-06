#include "server/webserver.h"

const int PORT = 10086;
const int logLevel = 1;
const char* logDir = "log";

int main(){

    WebServer server(PORT, 1, logLevel, logDir);
    server.Run();   
}