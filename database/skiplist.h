#ifndef SKIP_LIST_H_
#define SKIP_LIST_H_

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define SAVE_PATH   "log/data.log"

namespace mylist{

/*=============================class Node================================*/
template<class K, class V>
class Node{
public:
    Node() ;
    Node(K k, V v, int h){
        key_ = k;
        value_ = v;
        height = h; 

        forward = new Node<K, V>*[height + 1];
    
        memset(forward, 0, sizeof(Node<K, V>*) * (height + 1));
    }
    ~Node() {delete []forward;}

    K get_key() const {return key_;}
    V get_value() const {return value_;}

    void set_value(V v) {value_=v;}

    Node<K, V> **forward;

public:
    int height;

private:
    K key_;
    V value_;
};


/*=============================class SkipList================================*/

template <typename K, typename V> 
class SkipList {

public: 
    SkipList(int max_height){
        max_height_ = max_height;
        skip_list_height_ = 0;
        element_count_ = 0;
        K k;
        V v;
        header_ = new Node<K, V>(k, v, max_height);
    }
    ~SkipList(){
        if (file_writer_.is_open()) {file_writer_.close();}
        if (file_reader_.is_open()) {file_reader_.close();}
    }

    int get_random_height(){
        int h = 1;
        while (rand() % 2) {
            h++;
        }
        h = (h < max_height_) ? h : max_height_;
        return h;
    }

    bool insert_element(K, V);

    void delete_element(K);

    bool has_element(K);

    V get_element(K);

    void display_list();

    void dump_file(){
        file_writer_.open(SAVE_PATH);
        Node<K, V>* head = header_->forward[0]; 

        while (head != nullptr) {
        file_writer_ << head->get_key() << ":" << head->get_value() << "\n";
        head = head->forward[0];
        }
        file_writer_.flush();
        file_writer_.close();
    }
    // void load_file(){
    //     file_reader_.open(SAVE_PATH);
    //     std::string line;
    //     std::string* key = new std::string();
    //     std::string* value = new std::string();

    //     while (getline(file_reader_, line)) {
    //         get_key_value_from_string_(line, key, value);
    //         if (key->empty() || value->empty()) {
    //             continue;
    //         }
    //         insert_element(*key, *value);
    //         std::cout << "key:" << *key << "value:" << *value << std::endl;
    //     }
    //     file_reader_.close();
    // }

    size_t size(){
        return element_count_;
    }

private:
    void get_key_value_from_string_(const std::string& str, std::string* key, std::string* value);
//     bool is_valid_string_(const std::string& str);

    Node<K, V>* create_node_(K k, V v, int h){
        Node<K, V> *n = new Node<K, V>(k, v, h);
        return n;
    }
private:    

    std::mutex mtx_;

    // Maximum height of the skip list 
    int max_height_;

    // current_node height of skip list 
    int skip_list_height_;

    // pointer to header node 
    Node<K, V>* header_;

    // file operator
    std::ofstream file_writer_;
    std::ifstream file_reader_;

    // skiplist current_node element count
    int element_count_;
};


template<typename K, typename V>
bool SkipList<K, V>::insert_element(const K key, const V value) {
    
    std::lock_guard<std::mutex> locker(mtx_);

    Node<K, V> *current_node = header_;
    Node<K, V> *update_nodes[max_height_ + 1];
    memset(update_nodes, 0, sizeof(Node<K, V>*)*(max_height_ + 1));  

    for(int i = skip_list_height_; i >= 0; i--) {
        while(current_node->forward[i] != nullptr && current_node->forward[i]->get_key() < key) {
            current_node = current_node->forward[i]; 
        }
        update_nodes[i] = current_node;
    }

    //maybe key is already in the skiplist
    current_node = current_node->forward[0];
    
    if (current_node != nullptr && current_node->get_key() == key) {
        // std::cout << "key: " << key << ", exists" << std::endl;
        // delete_element(key);
        // return insert_element(key, value);
        current_node->set_value(value);
        return true;
    }

    int random_height = get_random_height();

    if (random_height > skip_list_height_) {
        for (int i = skip_list_height_ + 1; i < random_height + 1; i++) {
            update_nodes[i] = header_;
        }
        skip_list_height_ = random_height;
    }

    Node<K, V>* inserted_node = create_node_(key, value, random_height);

    for (int i = 0; i <= random_height; i++) {
        inserted_node->forward[i] = update_nodes[i]->forward[i];
        update_nodes[i]->forward[i] = inserted_node;
    }

    element_count_ ++;
    return true;
}

template<typename K, typename V> 
void SkipList<K, V>::delete_element(K key) {
    std::lock_guard<std::mutex> locker(mtx_);

    Node<K, V> *current_node = header_;
    Node<K, V> *update_nodes[max_height_ + 1];

    for(int i = skip_list_height_; i >=0; i--){
         while(current_node->forward[i] != nullptr && current_node->forward[i]->get_key() < key) {
            current_node = current_node->forward[i]; 
        }
        //current_node is the last node who's key_ < key
        update_nodes[i] = current_node;
    }

    //current_node is the first node who's key_ >= key
    current_node = current_node->forward[0];

    if (current_node != nullptr && current_node->get_key() == key) {
        for (int i = 0; i <= skip_list_height_; i++) {
            if (update_nodes[i]->forward[i] != current_node) 
                break;
            update_nodes[i]->forward[i] = current_node->forward[i];
        }
        while (skip_list_height_ > 0 && header_->forward[skip_list_height_] == nullptr) {
            skip_list_height_ --; 
        }
        // std::cout << "Successfully deleted key "<< key << std::endl;
        element_count_ --;

        delete current_node;
        current_node = nullptr;
    }
    return;
}

template<typename K, typename V> 
bool SkipList<K, V>::has_element(K key) {
    //no need a lock
    Node<K, V> *current_node = header_;

    for(int i = skip_list_height_; i >=0; i--){
         while(current_node->forward[i] != nullptr && current_node->forward[i]->get_key() < key) {
            current_node = current_node->forward[i]; 
        }
    }

    current_node = current_node->forward[0];

    if (current_node != nullptr && current_node->get_key() == key){
        return true;
    }else{
        return false;
    }
}

template<typename K, typename V> 
V SkipList<K, V>::get_element(K key) {
    Node<K, V> *current_node = header_;

    for(int i = skip_list_height_; i >=0; i--){
         while(current_node->forward[i] != nullptr && current_node->forward[i]->get_key() < key) {
            current_node = current_node->forward[i]; 
        }
    }

    current_node = current_node->forward[0];
    if (current_node != nullptr && current_node->get_key() == key){
        return current_node->get_value();
    }else{
        // std::cout << "Can't find key "<< key << std::endl;
        return V(0);
    }
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string_(const std::string& str, std::string* key, std::string* value) {

    *key = str.substr(0, str.find(":"));
    *value = str.substr(str.find(":")+1, str.length());
}


}//namespace skiplist

#endif //SKIP_LIST_H_