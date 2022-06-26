#include "database/doublelinklist.h"

#include <iostream>
#include <string>
#include "gtest/gtest.h"

TEST(testDoubleLinkList, test0){
    DoubleLink<int> list;
    DNode<int>* node1 = new DNode<int>(1);
    DNode<int>* node2 = new DNode<int>(2);
    list.Add2Tail(node1);
    list.Add2Tail(node2);
    EXPECT_EQ(list.getHead(), node1);
    EXPECT_EQ(list.getTail(), node2);
    EXPECT_EQ(list.size(), 2);

    DNode<int>* rm = list.RemoveFront();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(rm, node1);
    EXPECT_EQ(list.getHead(), node2);
    EXPECT_EQ(list.getTail(), node2);
    delete rm;
    rm = nullptr;

    DNode<int>* node3 = new DNode<int>(3);
    DNode<int>* node4 = new DNode<int>(4);
    DNode<int>* node5 = new DNode<int>(5);
    DNode<int>* node6 = new DNode<int>(6);
    list.Add2Tail(node3);
    list.Add2Tail(node4);
    list.Add2Tail(node5);
    list.Add2Tail(node6);
    list.Move2Tail(node3);
    EXPECT_EQ(list.getTail(), node3);
    EXPECT_EQ(list.size(), 5);

    list.RemoveNode(node6);
    EXPECT_EQ(list.getTail()->prev, node5);
}
