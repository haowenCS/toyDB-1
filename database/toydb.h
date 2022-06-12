#ifndef TOY_DB_H
#define TOY_DB_H

#include <string>
#include <vector>

enum{
    STRING = 0,
    SORTLIST,
    SET,
    HASH,
}ValueType;

struct PairObject{
    std::String key;
    ValueType value_type;
    void* value_ptr;
};

class ToyDB{
public:
    ToyDB() = default;
    ~ToyDB() = default;

    // bool insert_pair(PairObject obj){
    // }

    // bool delete_pair_by_key(std::string key){
    // }

private:
    std::vector<PairObject> pairs_;
}

#endif //TOY_DB_H