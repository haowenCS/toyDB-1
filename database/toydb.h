#ifndef TOY_DB_H
#define TOY_DB_H

#include <string>
#include <memory>
#include <map>

namespace toydb{

enum ValueType{
    DOUBLE = 0,
    STRING,
    DOUBLE_LIST,
    STRING_LIST,
    SET,
    HASH,
};


struct ValueObject{
    ValueType value_type;
    union Value{
       double double_type;
       void* ptr_type; 
    }value;
};
}   //namespace toyDB
#endif //TOY_DB_H