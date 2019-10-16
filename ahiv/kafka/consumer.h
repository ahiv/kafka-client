// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_CONSUMER_H
#define AHIV_KAFKA_CLIENT_CONSUMER_H

#include "ahiv/kafka/connection.h"
#include "ahiv/kafka/internal/topic.h"
#include "ahiv/kafka/protocol/packet/metadata.h"
#include "uvw.hpp"

namespace ahiv::kafka {
class Consumer : public Connection {
 public:
  Consumer(std::shared_ptr<uvw::Loop>& loop) : Connection(loop) {
    this->Once<ConnectedEvent>([this](const ConnectedEvent& event, auto&) {
      this->requestMetadataForTopics();
    });
  }

  // ConsumeFromTopic needs to be setup first before bootstrapping the consumer
  // itself. Adding topics after bootstrap is not supported and will not
  // subscribe them
  void ConsumeFromTopic(const std::string& topic) {
    this->wantedTopics.emplace_back(topic);
  }

  // AutoCreateTopics enables automatically creating topics not know to kafka.
  // This also depends on the server setting and may not result in anything.
  // Changing this value after bootstrap if not supported and will change
  // nothing
  void AutoCreateTopics(bool value) { this->autoCreate = value; }

 private:
  void requestMetadataForTopics() {
    this->startTopicMetadata = std::chrono::high_resolution_clock::now();

    ahiv::kafka::protocol::packet::MetadataRequestPacket requestPacket =
        ahiv::kafka::protocol::packet::MetadataRequestPacket(
            this->wantedTopics, this->autoCreate, false, false);

    ahiv::kafka::protocol::Buffer buffer;
    buffer.EnsureAllocated(requestPacket.Size());
    requestPacket.Write(buffer);

    this->SendToFirstConnection(buffer, [this](
                                            protocol::Buffer& responseBuffer) {
        ahiv::kafka::protocol::packet::MetadataResponsePacket metadataResponsePacket;
        metadataResponsePacket.Read(responseBuffer);

        for (auto broker : metadataResponsePacket.brokers) {
            this->consumeFromMetadata(broker);
        }

      std::chrono::duration<double> elapsed =
          std::chrono::high_resolution_clock::now() - this->startTopicMetadata;
      std::cout << "Getting topic meta took " << elapsed.count() << " s"
                << std::endl;
    });
  }

  std::vector<internal::Topic> topics;
  std::vector<std::string> wantedTopics;
  bool autoCreate;

  std::chrono::time_point<std::chrono::high_resolution_clock>
      startTopicMetadata;
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_CONSUMER_H
