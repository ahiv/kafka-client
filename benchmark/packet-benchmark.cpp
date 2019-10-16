#include "benchmark/benchmark.h"
#include "ahiv/kafka/protocol/packet/metadata.h"

static void MetadataWriter(benchmark::State& state) {
    // Perform setup here
    for (auto _ : state) {
        std::vector<std::string> topics =
                std::vector<std::string>({"test", "test1"});

        ahiv::kafka::protocol::packet::MetadataRequestPacket requestPacket =
                ahiv::kafka::protocol::packet::MetadataRequestPacket(topics, false, false, false);

        ahiv::kafka::protocol::Buffer buffer;
        buffer.EnsureAllocated(requestPacket.Size());
        requestPacket.Write(&buffer);

        benchmark::DoNotOptimize(buffer);
    }
}

// Register the function as a benchmark
BENCHMARK(MetadataWriter)->RangeMultiplier(2)->Range(1<<18, 1<<30)->Threads(2);

// Run the benchmark
BENCHMARK_MAIN();