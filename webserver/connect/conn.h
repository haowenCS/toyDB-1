#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>      

#include "webserver/logger/log.h"

class Conn {
public:
    Conn();

    ~Conn();

    void init(int sockFd, const sockaddr_in& addr);

    void Close();

    int GetFd() const;
    int GetPort() const;
    const char* GetIP() const;
    sockaddr_in GetAddr() const;
    

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;
    
private:
   
    int fd_;
    struct  sockaddr_in addr_;

    bool isClose_;
};


#endif //HTTP_CONN_H