#include "database/skiplist.h"
#include "database/toydb.h"

#include <iostream>
#include <string>
#include "gtest/gtest.h"

using namespace mylist;

TEST(testSkipList, test0){

    SkipList<std::string, toydb::ValueObject*> skipList(10);
    
    toydb::ValueObject* object = new toydb::ValueObject();
    object->value_type = toydb::DOUBLE;
    std::string str_("1");
    double val = stod(str_);
    object->value.double_type = val;

    toydb::ValueObject* object2 = new toydb::ValueObject();
    object2->value_type = toydb::STRING;
    object2->value.ptr_type = static_cast<void*>(new std::string("abc"));

    skipList.insert_element("a", object);
    skipList.insert_element("b", object2);


    bool b = skipList.has_element("a");
    toydb::ValueObject* object_1 = skipList.get_element("a");
    toydb::ValueObject* object_2 = skipList.get_element("b");

    EXPECT_EQ(b, true);
    EXPECT_EQ(object_1->value_type, toydb::DOUBLE);
    EXPECT_EQ(object_2->value_type, toydb::STRING);
}


TEST(testSkipList, test1){

    SkipList<std::string, toydb::ValueObject*> skipList(10);
    
    toydb::ValueObject* object = new toydb::ValueObject();
    object->value_type = toydb::DOUBLE;
    std::string str_("1");
    double val = stod(str_);
    object->value.double_type = val;

    toydb::ValueObject* object2 = new toydb::ValueObject();
    object2->value_type = toydb::STRING;
    object2->value.ptr_type = static_cast<void*>(new std::string("abc"));

    skipList.insert_element("a", object);
    skipList.insert_element("b", object2);
    skipList.insert_element("b", object);  

    bool b = skipList.has_element("a");
    toydb::ValueObject* object_1 = skipList.get_element("a");
    toydb::ValueObject* object_2 = skipList.get_element("b");

    EXPECT_EQ(b, true);
    EXPECT_EQ(object_1->value_type, toydb::DOUBLE);
    EXPECT_EQ(object_2->value_type, toydb::DOUBLE);

    skipList.delete_element("a");
    b = skipList.has_element("a");
    EXPECT_EQ(b, false);
}