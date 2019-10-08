package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])  # BSD/MIT.

cc_library(
    name = "uvw",
    hdrs = ["src/uvw.hpp"] + glob(["src/uvw/*.hpp"]),
    strip_include_prefix = "src",
    deps = [
        "@com_github_libuv_libuv//:libuv"
    ]
)