load("@rules_cc//cc:defs.bzl", "cc_proto_library") 
load("@rules_proto//proto:defs.bzl", "proto_library")


cc_proto_library( 
    name = "text_msg_cc_pb", 
    deps = [":text_msg_pb"], 
) 

proto_library( 
    name = "text_msg_pb", 
    srcs = ["text_msg.proto"], 
) 


cc_proto_library( 
    name = "database_msg_cc_pb", 
    deps = [":database_msg_pb"], 
) 

proto_library( 
    name = "database_msg_pb", 
    srcs = ["database_msg.proto"], 
) 

package(default_visibility = ["//visibility:public"]) 