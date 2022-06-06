#include "conn.h"
using namespace std;

const char* Conn::srcDir;
std::atomic<int> Conn::userCount;
bool Conn::isET;

Conn::Conn() { 
    fd_ = -1;
    addr_ = { 0 };
    isClose_ = true;
};

Conn::~Conn() { 
    Close(); 
};

void Conn::init(int fd, const sockaddr_in& addr) {
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    isClose_ = false;
    LOG(INFO, "Conn[%d](%s:%d) in, userCount:%d\n", fd_, GetIP(), GetPort(), (int)userCount);
}

void Conn::Close() {
    if(isClose_ == false){
        isClose_ = true; 
        userCount--;
        close(fd_);
        LOG(INFO, "Conn[%d](%s:%d) quit, UserCount:%d\n", fd_, GetIP(), GetPort(), (int)userCount);
    }
}

int Conn::GetFd() const {
    return fd_;
};

struct sockaddr_in Conn::GetAddr() const {
    return addr_;
}

const char* Conn::GetIP() const {
    return inet_ntoa(addr_.sin_addr);
}

int Conn::GetPort() const {
    return addr_.sin_port;
}