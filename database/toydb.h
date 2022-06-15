#ifndef TOY_DB_H
#define TOY_DB_H

#include <string>
#include <memory>
#include <map>
#include "server/logger/log.h"

namespace toydb{

enum ValueType{
    NONE = 0,
    DOUBLE,
    STRING,
    DOUBLE_LIST,
    STRING_LIST,
    SET,
    HASH,
};


class ValueObject{
public:
    ValueObject(){ value_type = NONE;}
    ~ValueObject(){
        if(value_type == STRING){
            delete static_cast<std::string*>(value.ptr_type);
        }else if(value_type == DOUBLE_LIST){
            delete static_cast<std::vector<double>*>(value.ptr_type);
        }else if(value_type == STRING_LIST){
            delete static_cast<std::vector<std::string>*>(value.ptr_type);
        }
    }

    ValueType value_type;
    union{
       double double_type;
       void* ptr_type; 
    }value;
};
}   //namespace toyDB
#endif //TOY_DB_H