#ifndef SKIP_LIST_H_
#define SKIP_LIST_H_

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <vector>
#include "database/toydb.h"

#define SAVE_PATH   "./depository/lastet.data"

namespace toydb{

// bool Type_inference(std::string str){
//     if(str.length() >= 2 && str[0] == '\"' && str[str.length()-1] == '\"')
//         return true;
//     return false;
// }

// bool Type_inference(char* chs){
//     return Type_inference(std::string(chs));
// }
/*=============================class Node================================*/
template<class K>
class Node{
public:
    Node() ;
    Node(K k, ValueObject* v, int h){
        key_ = k;
        value_ = v;
        height = h; 

        forward = new Node<K>*[height + 1];
    
        memset(forward, 0, sizeof(Node<K>*) * (height + 1));
    }
    ~Node() {delete []forward;}

    K get_key() const {return key_;}
    ValueObject* get_value() const {return value_;}

    void set_value(ValueObject* v) {value_=v;}

    Node<K> **forward;

public:
    int height;

private:
    K key_;
    ValueObject* value_;
};


/*=============================class SkipList================================*/

template <class K> 
class SkipList {

public: 
    SkipList(int max_height){
        max_height_ = max_height;
        skip_list_height_ = 0;
        element_count_ = 0;
        K k;
        ValueObject* v;
        header_ = new Node<K>(k, v, max_height);
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

    bool insert_element(K, ValueObject*);

    void delete_element(K);

    bool has_element(K);

    ValueObject* get_element(K);

    void display_list();

    void dump_file(){
        file_writer_.open(SAVE_PATH);
        Node<K>* node = header_->forward[0]; 

        while (node != nullptr) {
            file_writer_ << node->get_key() << ":";
            if(node->get_value()->value_type == DOUBLE){
                file_writer_ << node->get_value()->value.double_type;
            }
            else if(node->get_value()->value_type == STRING){
                file_writer_ << *static_cast<std::string*>(node->get_value()->value.ptr_type);
            }
            else if(node->get_value()->value_type == DOUBLE_LIST){
                std::vector<double> vec = *static_cast<std::vector<double>*>(node->get_value()->value.ptr_type);
                for(int i = 0; i < vec.size(); i++){
                    file_writer_ << vec[i]<<" ";
                }
            }
            else if(node->get_value()->value_type == STRING_LIST){
                std::vector<std::string> vec = *static_cast<std::vector<std::string>*>(node->get_value()->value.ptr_type);
                 for(int i = 0; i < vec.size(); i++){
                    file_writer_ << vec[i]<<" ";
                }
            }
            file_writer_ << "\n";
            node = node->forward[0];
        }
        file_writer_.flush();
        file_writer_.close();
    }
    void load_file(){
        file_reader_.open(SAVE_PATH);
        std::string line;
        std::string key;
        std::string value;

        while (getline(file_reader_, line)) {
            get_key_value_from_string_(line, key, value);
            
            ValueObject* object = new ValueObject();
            
            char buf[1024];
            bzero(&buf, sizeof(buf));
            strcpy(buf,value.c_str());
            char *buf_split[10], *p;
            p = NULL;

            p = strtok(buf, " ");
            int cnt = 0;
            while(p){
                buf_split[cnt] = p;
                ++cnt;
                p = strtok(NULL, " ");   
            }

            bool is_string = false;
            if(std::string(buf_split[0]).length() >= 2 && std::string(buf_split[0])[0] == '\"' \
                        && std::string(buf_split[0])[std::string(buf_split[0]).length()-1] == '\"'){
                is_string = true;
            }

            if(!is_string && cnt == 1){
                object->value_type = DOUBLE;
                object->value.double_type = stod(std::string(buf_split[0]));
                LOG(INFO,"Load Key:%s Value(double):%s\n",key.c_str(), value.c_str());
            }
            else if(is_string && cnt == 1){
                object->value_type = STRING;
                std::string* str = new std::string(buf_split[0]);
                object->value.ptr_type = static_cast<void*>(str);
                LOG(INFO,"Load Key:%s Value(string):%s\n",key.c_str(), value.c_str());
            }else if(!is_string && cnt > 1){
                object->value_type = DOUBLE_LIST;
                std::vector<double>* vec = new std::vector<double>(cnt);
                for(int i = 0; i < cnt; i++){
                    (*vec)[i] = stod(std::string(buf_split[i]));
                }
                object->value.ptr_type = static_cast<void*>(vec);
                LOG(INFO,"Load Key:%s Value(double list):%s\n",key.c_str(), value.c_str());
            }else if(is_string && cnt > 1){
                object->value_type = STRING_LIST;
                std::vector<std::string>* vec = new std::vector<std::string>(cnt);
                for(int i = 0; i < cnt; i++){
                    (*vec)[i] = std::string(buf_split[i]);
                }
                object->value.ptr_type = static_cast<void*>(vec);
                LOG(INFO,"Load Key:%s Value(string list):%s\n",key.c_str(), value.c_str());
            }
            insert_element(key, object);
        }
        file_reader_.close();
    }

    size_t size(){
        return element_count_;
    }

private:
    void get_key_value_from_string_(const std::string& str, std::string& key, std::string& value);
//     bool is_valid_string_(const std::string& str);

    Node<K>* create_node_(K k, ValueObject* v, int h){
        Node<K> *n = new Node<K>(k, v, h);
        return n;
    }
private:    

    std::mutex mtx_;

    // Maximum height of the skip list 
    int max_height_;

    // current_node height of skip list 
    int skip_list_height_;

    // pointer to header node 
    Node<K>* header_;

    // file operator
    std::ofstream file_writer_;
    std::ifstream file_reader_;

    // skiplist current_node element count
    int element_count_;
};


template<class K>
bool SkipList<K>::insert_element(const K key, ValueObject* value) {
    
    std::lock_guard<std::mutex> locker(mtx_);

    Node<K> *current_node = header_;
    Node<K> *update_nodes[max_height_ + 1];
    memset(update_nodes, 0, sizeof(Node<K>*)*(max_height_ + 1));  

    for(int i = skip_list_height_; i >= 0; i--) {
        while(current_node->forward[i] != nullptr && current_node->forward[i]->get_key() < key) {
            current_node = current_node->forward[i]; 
        }
        update_nodes[i] = current_node;
    }

    //maybe key is already in the skiplist
    current_node = current_node->forward[0];
    
    if (current_node != nullptr && current_node->get_key() == key) {
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

    Node<K>* inserted_node = create_node_(key, value, random_height);

    for (int i = 0; i <= random_height; i++) {
        inserted_node->forward[i] = update_nodes[i]->forward[i];
        update_nodes[i]->forward[i] = inserted_node;
    }
    element_count_ ++;
    return true;
}

template<class K> 
void SkipList<K>::delete_element(K key) {
    std::lock_guard<std::mutex> locker(mtx_);

    Node<K> *current_node = header_;
    Node<K> *update_nodes[max_height_ + 1];

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

template<class K> 
bool SkipList<K>::has_element(K key) {
    //no need a lock
    Node<K> *current_node = header_;

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

template<class K> 
ValueObject* SkipList<K>::get_element(K key) {
    Node<K> *current_node = header_;

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
        return nullptr;
    }
}

template<class K>
void SkipList<K>::get_key_value_from_string_(const std::string& str, std::string& key, std::string& value) {

    key = str.substr(0, str.find(":"));
    value = str.substr(str.find(":")+1, str.length());
}


}//namespace skiplist

#endif //SKIP_LIST_H_