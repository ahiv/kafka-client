cc_binary(
    name = "benchmark",
    srcs = glob(["**/*-benchmark.cpp"]),
    deps = [
        "@com_google_benchmark//:benchmark",
        "//ahiv/kafka:kafka-library-client",
    ],
)