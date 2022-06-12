load("@rules_cc//cc:defs.bzl", "cc_proto_library") 
load("@rules_proto//proto:defs.bzl", "proto_library") 

cc_binary(
	name = "server",
	srcs = ["main.cc"],
	deps = [
		":server_lib",
	],
)


cc_library(
    name = "server_lib",
    srcs = glob(["server/**/*.cc"]),
    hdrs = glob(["server/**/*.h"]),
	deps = [
        ":connect",
		":threadpool",
		":logger",
        "//protoc:text_msg_cc_pb", 
        "//protoc:database_msg_cc_pb",
        "//database:skiplist",
        "//database:sortlist",
    ],
)


cc_library(
    name = "connect",
    srcs = glob(["connect/**/*.cc"]),
    hdrs = glob(["connect/**/*.h"]),
	deps = [
        ":timer",
		":logger",
    ],
)


cc_library(
    name = "timer",
    srcs = glob(["timer/**/*.cc"]),
    hdrs = glob(["timer/**/*.h"]),
)


cc_library(
    name = "threadpool",
    hdrs = glob(["threadpool/**/*.h"]),
	linkopts = ["-lpthread"],
)


cc_library(
    name = "logger",
    srcs = glob(["logger/**/*.cc"]),
    hdrs = glob(["logger/**/*.h"]),
	linkopts = ["-lpthread"],
)

package(default_visibility = ["//visibility:public"]) 
