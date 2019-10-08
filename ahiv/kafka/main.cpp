#include "connectionconfig.h"
#include "consumer.h"
#include "loguru.hpp"
#include "uvw.hpp"

int main() {
  auto loop = uvw::Loop::getDefault();

  ahiv::kafka::Consumer consumer(loop);
  consumer.on<ahiv::kafka::ErrorEvent>(
      [](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
        LOG_F(ERROR, "Got error: %s", errorEvent.reason.c_str());
      });
  consumer.Bootstrap({"plaintext://localhost:9092"});

  loop->run();
}