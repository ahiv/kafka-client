#include "benchmark/benchmark.h"
#include "ahiv/kafka/protocol/packet.h"

static void MetadataWriter(benchmark::State& state) {
    // Perform setup here
    for (auto _ : state) {
        std::vector<std::string> topics =
                std::vector<std::string>({"test", "test1"});

        ahiv::kafka::protocol::MetadataRequestPacket requestPacket =
                ahiv::kafka::protocol::MetadataRequestPacket(topics, false, false, false);

        ahiv::kafka::protocol::Buffer buffer;
        buffer.EnsureAllocated(requestPacket.Size());
        requestPacket.Write(&buffer);

        benchmark::DoNotOptimize(buffer);
    }
}
// Register the function as a benchmark
BENCHMARK(MetadataWriter);

// Run the benchmark
BENCHMARK_MAIN();