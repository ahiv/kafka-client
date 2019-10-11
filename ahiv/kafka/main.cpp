#include "ahiv/kafka/protocol/packet.h"
#include <iostream>

int main() {
  std::vector<std::string> topics =
      std::vector<std::string>({"test", "test1"});

  ahiv::kafka::protocol::MetadataRequestPacket requestPacket =
      ahiv::kafka::protocol::MetadataRequestPacket(topics, false, false, false);

  ahiv::kafka::protocol::Buffer buffer;
  buffer.EnsureAllocated(requestPacket.Size());
  requestPacket.Write(&buffer);

  std::cout << buffer.Size() << std::endl << std::flush;
  for (int j = 0; j < buffer.Size(); j++) printf("%02X ", buffer.Data()[j]);

  std::cout << std::flush;
}