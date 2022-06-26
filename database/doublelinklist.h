#ifndef DOUBLE_LINK_H_
#define DOUBLE_LINK_H_

#include <string>

template<typename T>
struct DNode {
public:
    DNode<T>* prev;
    DNode<T>* next;

    T value;
public:
    DNode() {
        this->value = T(NULL);
		this->prev  = nullptr;
		this->next  = nullptr;
    }
	DNode(T t) {
		this->value = t;
		this->prev  = nullptr;
		this->next  = nullptr;
	}
};


template<typename T>
class DoubleLink {
public:
    DoubleLink(){}
    ~DoubleLink(){}

    bool is_empty();
    int size();

    DNode<T>* RemoveFront();
    void RemoveNode(DNode<T>* n);
    void Move2Tail(DNode<T>* n);
    void Add2Tail(DNode<T>* n);

    DNode<T>* getHead();
    DNode<T>* getTail();

private:
    int node_count_ = 0;
    DNode<T>* head_ = nullptr;
    DNode<T>* tail_ = nullptr;
};

template<typename T>
DNode<T>* DoubleLink<T>::getHead(){
    return head_;
}

template<typename T>
DNode<T>* DoubleLink<T>::getTail(){
    return tail_;
}

template<typename T>
bool DoubleLink<T>::is_empty(){
    if (node_count_ == 0){
        return true;
    } else {
        return false;
    }
}

template<typename T>
int DoubleLink<T>::size(){
    return node_count_;
}

template<typename T>
void DoubleLink<T>::Add2Tail(DNode<T>* n){
    if (node_count_ == 0) {
        n->prev = n;
        n->next = n;
        head_ = n;
        tail_ = n;
        ++node_count_;
        return;
    } else {
        ++node_count_;
        DNode<T>* old_tail = tail_;
        tail_ = n;
        tail_->prev = old_tail;
        old_tail->next = tail_;
        tail_->next = head_;
        head_->prev = tail_;
        return;
    }
}

template<typename T>
DNode<T>* DoubleLink<T>::RemoveFront(){
    if (node_count_ == 0) {
        return nullptr;
    } else if (node_count_ == 1) {
        DNode<T>* old_head = head_;
        head_ = nullptr;
        tail_ = nullptr;
        node_count_--;
        return old_head;
    } else {
        DNode<T>* old_head = head_;
        head_ = old_head->next;
        head_->prev = tail_;
        tail_->next = head_;
        node_count_--;
        return old_head;
    }
}

template<typename T>
void DoubleLink<T>::RemoveNode(DNode<T>* n){
    if (node_count_ <= 1) {
        node_count_ = 0;
        tail_ = nullptr;
        head_ = nullptr;
    } else {
        if (n == tail_) {
            tail_ = n->prev;
            tail_->next = head_;
            head_->prev = tail_;
        } else if (n == head_) {
            head_ = n->next;
            head_->prev = tail_;
            tail_->next = head_;
        } else {
            n->prev->next = n->next;
            n->next->prev = n->prev;
        }
    }
    node_count_--;
    return;
}

template<typename T>
void DoubleLink<T>::Move2Tail(DNode<T>* n){
    RemoveNode(n);
    Add2Tail(n);    
}

#endif //DOUBLE_LINK_H_