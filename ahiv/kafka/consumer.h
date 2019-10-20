// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CONSUMER_H
#define AHIV_KAFKA_CONSUMER_H

#include "ahiv/kafka/connection.h"
#include "ahiv/kafka/internal/topic.h"
#include "ahiv/kafka/protocol/packet/metadata.h"
#include "uvw.hpp"

namespace ahiv::kafka {
class Consumer : public Connection {
 public:
  Consumer(std::shared_ptr<uvw::Loop>& loop) : Connection(loop) {
    this->Once<ConnectedEvent>([this](const ConnectedEvent& event, auto&) {
      this->requestMetadataForTopics(this->wantedTopics, this->autoCreate);
    });

    this->On<UpdateTopicInformationEvent>([this](const UpdateTopicInformationEvent& event, auto&) {
      this->updateTopicInformation(event);
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
  // updateTopicInformation takes the event from the connection when it found a new or updated topic in metadata
  void updateTopicInformation(const UpdateTopicInformationEvent& event) {

  }

  std::vector<internal::Topic> topics;
  std::vector<std::string> wantedTopics;
  bool autoCreate;
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CONSUMER_H
