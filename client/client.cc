#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <regex.h>

#include "protoc/database_msg.pb.h"
#include "protoc/text_msg.pb.h"


bool CommandParser(char** command, std::string& sender, int parameter_cnt){
    if(strcmp(command[0],"set") == 0 || strcmp(command[0],"SET") == 0 ){
        if(parameter_cnt != 2){
            printf("Wrong parameter of `SET`, expect 2 but %d\n", parameter_cnt);
            return false;
        }
        std::string key(command[1]);
        std::string value(command[2]);

        msg::DatabaseMsg request_msg;
        msg::InsertElementRequest request;
        request.set_key(key);
        request.set_value(value);
        request_msg.set_msg_type(msg::INSERT_ELEMENT_REQUEST);
        request_msg.mutable_insert_element_request()->CopyFrom(request);
        if(!request_msg.SerializeToString(&sender)){
            printf("Something wrong about SerializeToString");
        }
    }else{
        printf("Command %s is not supported%d\n", command[0]);
        return false;
    }
    return true;
}


int main(int argc, char** argv) {
    assert(argc >= 2);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(atoi(argv[1]));

    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error";
    

    printf(" _______          _____  ____\n");  
    printf("|__   __|        |  __ \\|  _ \\ \n");
    printf("   | | ___  _   _| |  | | |_) |\n");
    printf("   | |/ _ \\| | | | |  | |  _ <\n");
    printf("   | | (_) | |_| | |__| | |_) |\n");
    printf("   |_|\\___/ \\__, |_____/|____/\n");
    printf("             __/ |\n");         
    printf("            |___/\n");           



    while(true){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        printf("<");
        std::cin.getline(buf, 1024);

        char *buf_split[3], *p;
        p = NULL;
        p = strtok(buf, " ");
        int cnt = 0;
        while(p){
            buf_split[cnt] = p;
		    ++cnt;
            p = strtok(NULL, " ");   
        }

        std::string sender;
        if(!CommandParser(buf_split, sender, --cnt)){
            continue;
        }

        ssize_t write_bytes = write(sockfd, sender.c_str(), sender.length());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&buf  , sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));

        msg::DatabaseMsg response_msg;
        response_msg.ParseFromString(std::string(buf));

        if(response_msg.msg_type() == msg::INSERT_ELEMENT_RESPONSE){
            msg::InsertElementResponse response;
            response = response_msg.insert_element_response();
            printf("%s\n", response.status().c_str());
        }
    }
    close(sockfd);
    return 0;
}