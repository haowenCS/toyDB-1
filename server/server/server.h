#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include "epoller.h"

#include <memory>		 //unique_ptr
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <errno.h>
#include <iostream>
#include <stdexcept>

#include "server/connect/conn.h"
#include "server/threadpool/threadpool.h"
#include "server/timer/heaptimer.h"
#include "proto/text_msg.pb.h"
#include "proto/database_msg.pb.h"
#include "database/skiplist.h"
#include "database/sortlist.h"
#include "database/toydb.h"
#include "database/lru.h"



class Server{
public:
    /*端口, epoll激发模式*/
    explicit Server(int port, int trigMode, int logLevel, const char* logDir, const bool loadDepository);
    ~Server();

    void Run();

    static void SigHandler(int sig);
    friend void doTimeout(int fd, Server* webserver);

private:
    int port_;      //私有字段一般以 `_` 结尾
    int listenFd_;
    bool isClose_;
    static bool isTimeOut_;

    uint32_t listenEvent_;
    int timeoutMS_;  /* 毫秒MS,用于timer_ */

    static int pipefd_[2];

    using db_type = toydb::SkipList<std::string>;
    // using db_type = std::map<std::string, ValueObject>;

    std::unique_ptr<LruCacheSL> lruCache_;
    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<db_type> toyDB_;
    std::unique_ptr<HeapTimer> timer_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unordered_map<int, Conn> clients_;

    void InitSocket_(); 
    void InitEventMode_(int trigMode);
    void AddClient_(int fd, sockaddr_in addr);

    int SetNonblock(int fd);

    void DealConnect_();
    void DealWrite_(Conn* client);
    void DealRead_(Conn* client);
    void DealSig_();
    void CloseConn_(Conn* client);

    void DoRead_(Conn* client);

    void Addsig_(int sig);
    void TimerHandler_();

    void deal_put_msg_(msg::DatabaseMsg &request_msg, msg::DatabaseMsg &response_msg, std::string &sender);
    void deal_get_msg_(msg::DatabaseMsg &request_msg, msg::DatabaseMsg &response_msg, std::string &sender);
    void deal_del_msg_(msg::DatabaseMsg &request_msg, msg::DatabaseMsg &response_msg, std::string &sender);
};


#endif //WEBSERVER_H_
