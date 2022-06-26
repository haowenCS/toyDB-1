cc_library(
  name = "lru",
  srcs = ["lru.cc"],
  hdrs = ["lru.h"],
  deps = [
    ":toydb",
    ":skiplist",
    ":doublelinklist",
  ],
)

cc_library(
  name = "doublelinklist",
  srcs = ["doublelinklist.cc"],
  hdrs = ["doublelinklist.h"],
)


cc_library(
  name = "toydb",
  srcs = ["toydb.cc"],
  hdrs = ["toydb.h"],
  deps = [
    "//server:logger",
  ],
)


cc_library(
  name = "skiplist",
  hdrs = ["skiplist.h"],
  deps = [
    ":toydb",
  ],
)

cc_library(
  name = "sortlist",
  hdrs = ["sortlist.h"],
  deps = [
    ":skiplist",
  ],
)


package(default_visibility = ["//visibility:public"]) 