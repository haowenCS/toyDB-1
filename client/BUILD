load("@rules_cc//cc:defs.bzl", "cc_proto_library") 
load("@rules_proto//proto:defs.bzl", "proto_library") 

cc_binary(
	name = "client",
	srcs = ["client.cc"],
	deps = [ 
        "//protoc:text_msg_cc_pb", 
    ], 
)
