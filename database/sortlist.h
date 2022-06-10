#ifndef SORT_LIST_H_
#define SORT_LIST_H_

#include "skiplist.h"
#include <unordered_map>
#include <memory>
#include <vector>

namespace mylist{

template<typename K, typename V>
class SortList{
public:
    using slist_ptr_type = std::unique_ptr<SkipList<K, V>>;
    using hash_prt_type = std::unique_ptr<std::unordered_map<K, V>>;
    using slist_type = SkipList<K, V>;
    using hash_type = std::unordered_map<K, V>;

public:
    SortList(int max_height = 10);
    ~SortList(){}

    bool insert_element(K, V);

    void delete_element(K);

    bool has_element(K);

    V get_element(K);                           //单点查询
    void get_element(K, K, std::vector<V>&);    //范围查询

    size_t size(){
        assert(slist_->size() == hash_->size());
        return slist_->size();
    }

private:
    slist_ptr_type slist_;
    hash_prt_type hash_;
};

template<typename K, typename V>
SortList<K, V>::SortList(int max_height){
    slist_ = std::make_unique<slist_type>(max_height);
    hash_ = std::make_unique<hash_type>();
}

template<typename K, typename V>
bool SortList<K, V>::insert_element(const K key, const V value){
    hash_->emplace(key, value);
    return slist_->insert_element(key, value);
}




}

#endif //SORT_LIST_H_