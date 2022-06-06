cc_binary(
	name = "webserver",
	srcs = ["main.cc"],
	deps = [
		":server"
	],
)


cc_library(
    name = "server",
    srcs = glob(["server/**/*.cc"]),
    hdrs = glob(["server/**/*.h"]),
    visibility = ["//visibility:public"],
	deps = [
        ":connect",
		":threadpool",
		":logger",
    ],
)


cc_library(
    name = "connect",
    srcs = glob(["connect/**/*.cc"]),
    hdrs = glob(["connect/**/*.h"]),
    visibility = ["//visibility:public"],
	deps = [
        ":timer",
		":logger",
    ],
)


cc_library(
    name = "timer",
    srcs = glob(["timer/**/*.cc"]),
    hdrs = glob(["timer/**/*.h"]),
    visibility = ["//visibility:public"],
)


cc_library(
    name = "threadpool",
    hdrs = glob(["threadpool/**/*.h"]),
    visibility = ["//visibility:public"],
	linkopts = ["-lpthread"],
)


cc_library(
    name = "logger",
    srcs = glob(["logger/**/*.cc"]),
    hdrs = glob(["logger/**/*.h"]),
    visibility = ["//visibility:public"],
	linkopts = ["-lpthread"],
)