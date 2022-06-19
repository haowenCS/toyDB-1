load("@rules_cc//cc:defs.bzl", "cc_proto_library") 
load("@rules_proto//proto:defs.bzl", "proto_library") 
load("@io_bazel_rules_go//go:def.bzl", "go_binary")

cc_binary(
	name = "cc_client",
	srcs = ["client.cc"],
	deps = [ 
        "//proto:text_msg_cc_pb", 
		"//proto:database_msg_cc_pb",
		"//database:toydb",
    ], 
)

go_binary(
   name = "go_client",
   srcs = ["client.go"],
   importpath = "toydb/gp-client",
   deps = [ 
		"//proto:database_msg_go_pb"
   ],
)