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
	DNode(T t, DNode *prev, DNode *next) {
		this->value = t;
		this->prev  = prev;
		this->next  = next;
	}
};


template<typename T>
class DoubleLink {
public:
    DoubleLink();
    ~DoubleLink();

    bool is_empty();
    int size();

    DNode<T>* RemoveFront();
    bool Move2Tail(DNode<T>* key);
    bool Add2Tail(DNode<T>* key);

private:
    int node_count_ = 0;
    DNode<T>* head_;
    DNode<T>* tail_;
};


#endif //DOUBLE_LINK_H_