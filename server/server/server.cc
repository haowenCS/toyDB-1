#include <iostream>
#include "server.h"

const int TIMEOUT = 10;
const int BUFSIZE = 1024;

int WebServer::pipefd_[2];
bool WebServer::isTimeOut_ = false;


WebServer::WebServer(int port, int trigMode, int logLevel, const char* logDir) : port_(port), isClose_(false){
    listenEvent_ = EPOLLRDHUP | EPOLLIN; //断开连接 | 接受连接
    InitEventMode_(trigMode);
    InitSocket_();
    timer_ = std::unique_ptr<HeapTimer>(new HeapTimer());
    threadpool_ = std::unique_ptr<ThreadPool>(new ThreadPool(8));


    Log::Instance()->init(logLevel, logDir, ".log", 1024);
    if(isClose_) { LOG(ERROR,"========== Server init error!=========="); }
    else {
        LOG(INFO, "========== Server init ==========\n");
        LOG(INFO, "Port:%d \n", port_);
        LOG(INFO, "LogSys level: %d \n", logLevel);
    }

    skiplist_ = std::unique_ptr<skiplist_type>(new skiplist_type(10));
}

WebServer::~WebServer(){
    LOG(INFO, "========== Server offline ==========\n");
    isTimeOut_ = true;
    isClose_ = true;
    close(listenFd_);
}

void WebServer::InitEventMode_(int trigMode){
		epoller_ = std::unique_ptr<Epoller>(new Epoller);    
		switch (trigMode){
    case 0:
        break;
    case 1:
        listenEvent_ |= EPOLLET;
        break;
    default:
        listenEvent_ |= EPOLLET;
        break;
    }
}


void WebServer::InitSocket_(){
    int ret;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);

    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenFd_ > 0);

    ret = bind(listenFd_, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0){std::cerr << "bind()"<<std::endl; close(listenFd_);}

    ret = listen(listenFd_, 6);
    if(ret < 0){std::cerr << "listen()"<<std::endl; close(listenFd_);}
		
    ret = epoller_->AddFd(listenFd_, listenEvent_);
    if(ret < 0){std::cerr << "add_listen"<<std::endl; close(listenFd_);}

    SetNonblock(listenFd_);
}

int WebServer::SetNonblock(int fd){
    assert(fd >= 0);
    int oldOpt = fcntl(fd, F_GETFD, 0);
    int newOpt = fcntl(fd, F_SETFL, oldOpt |  O_NONBLOCK);
    return oldOpt;
}


void WebServer::Run(){
    Addsig_(SIGALRM);
    Addsig_(SIGTERM);
    alarm(TIMEOUT);
    // int retsocketpair = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd_);
    // assert(retsocketpair!=-1);
    // epoller_->AddFd(pipefd_[0], EPOLLIN | EPOLLET);
    // SetNonblock(pipefd_[1]);
    // SetNonblock(pipefd_[0]);

    while(!isClose_){
        int eventCnt = epoller_->Wait(TIMEOUT);
        for(int eventIdx = 0; eventIdx < eventCnt; eventIdx++){
            /*处理事件，注意要统一事件源*/
            int fd = epoller_->GetEventFd(eventIdx);
            // printf("event fd:%d\n",fd);
            uint32_t events = epoller_->GetEvent(eventIdx);
            if(fd == listenFd_){
                DealConnect_();
            }
            //else if(fd == pipefd_[0]){
            //     printf("HEART-BEAT--\n");
            //     DealSig_();
            // }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                // LOG(INFO ,"EPOLLRDHUP | EPOLLHUP | EPOLLERR--");
                CloseConn_(&clients_[fd]);
            }
            else if(events & EPOLLIN){
                // LOG(INFO ,"EPOLLIN--");
                assert(clients_.count(fd) > 0);
                DealRead_(&clients_[fd]);
            }
            else if(events & EPOLLOUT){
                // LOG(INFO ,"EPOLLOUT--");
                assert(clients_.count(fd) > 0);
                DealWrite_(&clients_[fd]);
            }
            else {
                LOG(INFO ,"Unexpected event--\n");
            }
        }
        if(isTimeOut_){
            TimerHandler_();
            isTimeOut_ = false;
        }

    }
}

void WebServer::CloseConn_(Conn* client){
    epoller_->DelFd(client->GetFd());
    client->Close();
}

void WebServer::DealRead_(Conn* client){
    threadpool_->AddTask(std::bind(&WebServer::DoRead_, this, client));
}

void WebServer::DoRead_(Conn* client){
    char buf[BUFSIZE];
    while(true){    //由于使用非阻塞IO，需要不断读取，直到全部读取完毕
        timer_->adjust(client->GetFd(), TIMEOUT*1000);
        int fd = client->GetFd();
        ssize_t bytes_read = read(fd, buf, sizeof(buf));

        msg::DatabaseMsg request_msg;
        msg::InsertElementRequest request;
        request_msg.ParseFromString(std::string(buf));
        request = request_msg.insert_element_request();
        if(request_msg.msg_type() == msg::INSERT_ELEMENT_REQUEST){
            LOG(INFO, "Request Key:%s Value:%s \n", request.key().c_str(), request.value().c_str());
            skiplist_->insert_element(request.key(), request.value());
        }
        if(bytes_read > 0){
            // LOG(INFO ,"message from client fd %d: %s\n", fd, buf);
            // write(fd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            //该fd上数据读取完毕
            break;
        } else if(bytes_read == 0){  //EOF事件，一般表示客户端断开连接
            timer_->del_(timer_->ref_[fd]);
            CloseConn_(client);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        } //剩下的bytes_read == -1的情况表示其他错误，这里没有处理
    }
}


void WebServer::DealWrite_(Conn* client){
    
}


void doTimeout(int fd, WebServer* webserver){
    LOG(INFO ,"client[%d]:timeout\n", fd);
    webserver->CloseConn_(&(webserver->clients_[fd]));
}

void WebServer::TimerHandler_(){
    timer_->tick();
    LOG(DEBUG ,"tick\n");
    alarm(TIMEOUT);
}


void WebServer::SigHandler(int sig){
    int msg = sig;
    isTimeOut_ = true;
    // if(msg == SIGALRM)
    //     printf("SigHandle:SIGALRM\n");
    // int ret = write(pipefd_[1], (char*)msg, 1);
    // printf("ret:%d\n",ret);
}

void SigHandler(int sig){
    WebServer::SigHandler(sig);
}

void WebServer::Addsig_(int sig){
    signal(sig, SigHandler);
}

void WebServer::DealConnect_(){
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int clientFd = accept(listenFd_, (struct sockaddr *)&addr, &len);
    epoller_->AddFd(clientFd, EPOLLIN | EPOLLET);
    SetNonblock(clientFd);

    assert(clientFd > 0);
    clients_[clientFd].init(clientFd, addr);



    timer_->add(clientFd, TIMEOUT*1000, std::bind(doTimeout, clientFd, this));
}

// void WebServer::DealSig_(){
//     int sig;
//     char signals[1024];
//     int ret = read(pipefd_[0], signals, sizeof(signals));
//     if(ret <= 0){
//         return;
//     }else{
//         for(int i = 0; i < ret; i++){
//             if(signals[i] == SIGALRM){
//                 isTimeOut_ = true; //最后再处理超时事件
//             }else if(signals[i] == SIGTERM){
//                 isClose_ = true;
//             }
//         }
//     }
// }