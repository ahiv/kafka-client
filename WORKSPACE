# Tooling for downloading from http and git
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# === benchmark
http_archive(
    name = "com_google_benchmark",
    urls = [
        "https://github.com/google/benchmark/archive/v1.5.0.zip",
    ],
    strip_prefix = "benchmark-1.5.0",
)

# === libuv
http_archive(
    name = "com_github_libuv_libuv",
    urls = [
        "https://github.com/libuv/libuv/archive/v1.32.0.zip",
    ],
    sha256 = "b776581a170c32e6dd42fb0c170ab7efa632aa4786ac8816bf1cbf03d9c9cb63",
    strip_prefix = "libuv-1.32.0",
    build_file = "@//bazel/third_party/libuv:BUILD",
)

# === libuv cpp wrapper
http_archive(
    name = "com_skypjack_uvw",
    urls = [
        "https://github.com/skypjack/uvw/archive/v2.1.0_libuv-v1.32.zip"
    ],
    sha256 = "5b3b12a544f49e47d76affce7aba5f850221e37779173bd897e9d5408eaf9840",
    strip_prefix = "uvw-2.1.0_libuv-v1.32",
    build_file = "@//bazel/third_party/uvw:BUILD"
)

# === logging
http_archive(
    name = "com_github_emilk_loguru",
    urls = [
        "https://github.com/emilk/loguru/archive/v2.1.0.zip",
    ],
    sha256 = "8d2d28969a4e91f8ef71ae4c1ba140097d274f75b6587fb4f465ce9434402fd2",
    strip_prefix = "loguru-2.1.0",
    build_file = "@//bazel/third_party/loguru:BUILD",
)

# === gtest
http_archive(
    name = "com_google_gtest",
    urls = [
        "https://github.com/google/googletest/archive/release-1.10.0.zip"
    ],
    strip_prefix = "googletest-release-1.10.0",
    sha256 = "94c634d499558a76fa649edb13721dce6e98fb1e7018dfaeba3cd7a083945e91",
)