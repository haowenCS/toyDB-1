#include "server/server.h"

const int PORT = 10086;
const int logLevel = 1;
const char* logDir = "log";
const bool loadDepository = true;

int main(){
    Server server(PORT, 1, logLevel, logDir, loadDepository);
    server.Run();   
}