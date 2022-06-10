cc_test(
  name = "test_skiplist",
  srcs = ["test_skiplist.cc"],
  deps = [
    ":skiplist",
    "@com_google_googletest//:gtest_main",
  ],
)


cc_test(
  name = "test_sortlist",
  srcs = ["test_sortlist.cc"],
  deps = [
    ":skiplist",
    ":sortlist",
    "@com_google_googletest//:gtest_main",
  ],
)

cc_library(
  name = "skiplist",
  hdrs = ["skiplist.h"],
)

cc_library(
  name = "sortlist",
  hdrs = ["sortlist.h"],
)