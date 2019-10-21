#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include "ahiv/kafka/consumer.h"

int main() {
  auto loop = uvw::Loop::getDefault();

  ahiv::kafka::Consumer consumer(loop);
  consumer.On<ahiv::kafka::ErrorEvent>(
      [](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
        std::cout << errorEvent.Reason << std::endl;
      });
  consumer.ConsumeFromTopic("test");
  consumer.Bootstrap({"plaintext://localhost:9092"});

  loop->run();
  std::cout << std::flush;
}