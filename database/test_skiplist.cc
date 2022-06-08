#include <iostream>

#include "gtest/gtest.h"
#include "skiplist.h"

using namespace skiplist;

TEST(testSkipList, test0){

    SkipList<int, std::string> skipList(10);
    
    skipList.insert_element(1, "hello");
    skipList.insert_element(2, ",");
    skipList.insert_element(3, "skiplist");

    skipList.delete_element(1);

    skipList.delete_element(1);

    std::string s;
    if(skipList.has_element(2)){
        s = skipList.get_element(2);
    }

    bool b1 = skipList.has_element(1);
    bool b2 = skipList.has_element(2);

    EXPECT_EQ(b1, false);
    EXPECT_EQ(b2, true);
    EXPECT_EQ(s, ",");

    skipList.dump_file();
    // skipList.load_file();
}