#ifndef LRU_H_
#define LRU_H_ 

#include "database/toydb.h"
#include "database/skiplist.h"
#include "database/doublelinklist.h"
#include <memory>
#include <string>
#include <unordered_map>

using keyType = std::string;
using valueType = toydb::ValueObject;
using nodeType = DNode<std::string>;

class LruCacheBase {
public:
    LruCacheBase(int maxCacheSize) : maxCacheSize_(maxCacheSize) {}
    virtual ~LruCacheBase() = 0;

    virtual bool Set(keyType) = 0;
    virtual void Del(keyType) = 0;
    virtual void Update() = 0;
    virtual valueType* Get(keyType) = 0;

    virtual void DumpFile() = 0;
    virtual void LoadFile() = 0;

private:
    std::unordered_map<keyType, nodeType> map_;
    DoubleLink<std::string> list;
    int maxCacheSize_;
    int curCacheSize_ = 0;
};

class LruCacheSL : public LruCacheBase{
public:
    using db_pointer = toydb::SkipList<keyType>*;
    using db_type = toydb::SkipList<keyType>;

    LruCacheSL(int maxCacheSize, db_pointer p) : LruCacheBase(maxCacheSize) {
        ptr_ = std::unique_ptr<db_type>(p);
    }
    ~LruCacheSL();

    bool Set(keyType);
    void Del(keyType);
    valueType* Get(keyType);

    void Update();

    void DumpFile();
    void LoadFile();

private:
    std::unique_ptr<db_type> ptr_;
} ;

class LruCacheUM : public LruCacheBase{
public:
    using db_pointer = std::unordered_map<keyType, valueType>*;
    using db_type = std::unordered_map<keyType, valueType>;

    LruCacheUM(int maxCacheSize, db_pointer p) : LruCacheBase(maxCacheSize) {
        ptr_ = std::unique_ptr<db_type>(p);
    }
    ~LruCacheUM();

    bool Set(keyType);
    void Del(keyType);
    valueType* Get(keyType);

    void Update();

    void DumpFile();
    void LoadFile();

private:
    std::unique_ptr<db_type> ptr_;
} ;

#endif //LRU_H_