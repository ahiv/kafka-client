cc_library(
    name = "kafka-library-client",
    srcs = glob(["**/*.h"]),
    deps = [
        "@com_skypjack_uvw//:uvw"
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "kafka-client",
    srcs = ["main.cpp"],
    deps = [
        ":kafka-library-client",
        "@com_github_emilk_loguru//:loguru"
    ],
)