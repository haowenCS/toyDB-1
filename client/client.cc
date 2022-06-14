#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <regex.h>
#include <stdexcept>


#include "protoc/database_msg.pb.h"
#include "protoc/text_msg.pb.h"
#include "database/toydb.h"

#define BUFSIZE     1024
#define VERSION     "v0.0.1"

static char command_cache[BUFSIZE];         //暂时没用到
static bool has_command_cache = false;      //暂时没用到
static bool use_command_cache = false;      //暂时没用到

bool Is_sting_type(std::string str){
    if(str.length() >= 2 && str[0] == '\"' && str[str.length()-1] == '\"')
        return true;
    return false;
}

bool Is_string_type(char* chs){
    return Is_sting_type(std::string(chs));
}

bool deal_set(char** command, std::string& sender, int parameter_cnt){
    if(parameter_cnt < 2){
            printf("ERROR, Wrong parameter of `SET`, expect more than 3 but %d\n", parameter_cnt);
            return false;
        }
        msg::DatabaseMsg request_msg;
        msg::InsertElementRequest request;
        std::string key(command[1]);
        request.set_key(key);

        bool is_string = Is_sting_type(command[2]);

        request.add_value(std::string(command[2]));
        if(parameter_cnt == 2 && !is_string){
            try{stod(std::string(command[2]));}
            catch(std::invalid_argument&){
                printf("ERROR, Values is requested to be correct type\n");
                return false;
            }
            request.set_value_type(msg::DOUBLE);
        }
        else if(parameter_cnt == 2 && is_string){
            request.set_value_type(msg::STRING);
        }
        else if(parameter_cnt > 2 && !is_string){
            request.set_value_type(msg::DOUBLE_LIST);
            for(int i = 3; i <= parameter_cnt; i++){
                if(Is_sting_type(command[i]) != is_string){
                    printf("ERROR, Values is requested to be correct type\n");
                    return false;
                }
                try{stod(std::string(command[i]));}
                catch(std::invalid_argument&){
                    printf("ERROR, Values is requested to be correct type\n");
                    return false;
                }
                request.add_value(std::string(command[i]));
            }
        }
        else if(parameter_cnt > 2 && is_string){
            request.set_value_type(msg::STRING_LIST);
            for(int i = 3; i <= parameter_cnt; i++){
                if(Is_sting_type(command[i]) != is_string){
                    printf("ERROR, Values is requested to be same type\n");
                    return false;
                }
                request.add_value(std::string(command[i]));
            }
        }
        request_msg.set_msg_type(msg::INSERT_ELEMENT_REQUEST);
        request_msg.mutable_insert_element_request()->CopyFrom(request);
        request_msg.SerializeToString(&sender);
    return true;
}

bool deal_get(char** command, std::string& sender, int parameter_cnt){
    if(parameter_cnt != 1){
            printf("ERROR, Wrong parameter of `GET`, expect 2 but %d\n", parameter_cnt);
            return false;
    }
    msg::DatabaseMsg request_msg;
    msg::GetElementRequest request;

    request.set_key(std::string(command[1]));
    request_msg.set_msg_type(msg::GET_ELEMENT_REQUEST);
    request_msg.mutable_get_element_request()->CopyFrom(request);
    request_msg.SerializeToString(&sender);

    return true;
}

bool deal_del(char** command, std::string& sender, int parameter_cnt){
    if(parameter_cnt != 1){
            printf("ERROR, Wrong parameter of `DEL`, expect 2 but %d\n", parameter_cnt);
            return false;
    }
    msg::DatabaseMsg request_msg;
    msg::DeleteElementRequest request;

    request.set_key(std::string(command[1]));
    request_msg.set_msg_type(msg::DELETE_ELEMENT_REQUEST);
    request_msg.mutable_delete_element_request()->CopyFrom(request);
    request_msg.SerializeToString(&sender);

    return true;
}

bool CommandParser(char** command, std::string& sender, int parameter_cnt){    
    if(strcmp(command[0],"") == 0){
        return false;
    }
    if(strcmp(command[0],"set") == 0 || strcmp(command[0],"SET") == 0 ){
        return deal_set(command, sender, parameter_cnt);
    }
    else if(strcmp(command[0],"get") == 0 || strcmp(command[0],"GET") == 0 ){
        return deal_get(command, sender, parameter_cnt);
    }
    else if(strcmp(command[0],"DEL") == 0 || strcmp(command[0],"del") == 0 ){
        return deal_del(command, sender, parameter_cnt);
    }
    else{
        printf("ERROR, Command %s is not supported\n", command[0]);
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
    

    printf(" _______          _____  ____\n ");  
    printf("|__   __|        |  __ \\|  _ \\                     author : Zhang Yu-qin\n ");
    printf("   | | ___  _   _| |  | | |_) |                    version: %s\n", VERSION);
    printf("    | |/ _ \\| | | | |  | |  _ <\n");
    printf("    | | (_) | |_| | |__| | |_) |\n");
    printf("    |_|\\___/ \\__, |_____/|____/\n");
    printf("              __/ |\n");         
    printf("             |___/\n");           


    while(true){
        char buf[BUFSIZE];
        bzero(&buf, sizeof(buf));
        printf("< ");

        if(!has_command_cache || !use_command_cache){
            std::cin.getline(buf, BUFSIZE);
            strcpy(command_cache, buf);
            has_command_cache = true;
        }else{
            strcpy(buf,command_cache);
            use_command_cache = false;
        }

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
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));

        msg::DatabaseMsg response_msg;
        response_msg.ParseFromString(std::string(buf));

        if(response_msg.msg_type() == msg::INSERT_ELEMENT_RESPONSE){
            msg::InsertElementResponse response;
            response = response_msg.insert_element_response();
            printf("%s\n", response.status().c_str());
        }
        else if(response_msg.msg_type() == msg::GET_ELEMENT_RESPONSE){
            msg::GetElementResponse response;
            response = response_msg.get_element_response();
            if(response.status() == "NIL"){
                printf("%s\n", response.status().c_str());
            }
            else{
                if(response.value_type() == msg::DOUBLE){
                    printf("(double) %s\n", response.value(0).c_str());
                }else if(response.value_type() == msg::STRING){
                    printf("(string) %s\n", response.value(0).c_str());
                }else if(response.value_type() == msg::DOUBLE_LIST){
                    printf("(double)");
                    for(int i = 0; i < response.value_size(); i++){
                        printf(" %s,", response.value(i).c_str());
                    }
                    printf("\n");
                }else if(response.value_type() == msg::STRING_LIST){
                    printf("(string)");
                    for(int i = 0; i < response.value_size(); i++){
                        printf(" %s,", response.value(i).c_str());
                    }
                    printf("\n");
                }
            }
        }
        else if(response_msg.msg_type() == msg::DELETE_ELEMENT_RESPONSE){
            msg::DeleteElementResponse response;
            response = response_msg.delete_element_response();
            printf("%s\n", response.status().c_str());
        }
    }
    close(sockfd);
    return 0;
}