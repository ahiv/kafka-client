// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_PROTOCOL_CLIENT_PACKET_H
#define AHIV_KAFKA_PROTOCOL_CLIENT_PACKET_H

#include "ahiv/kafka/protocol/buffer.h"

namespace ahiv::kafka::protocol {
struct BasePacket {
  std::vector<char>::iterator packetSizePosition;
  int32_t packetSize;

  virtual void Write(Buffer* buffer) {
      packetSizePosition = buffer->Write<int32_t>(0);
  }

  virtual std::size_t Size() { return 4; }
};

struct RequestPacket : public BasePacket {
  RequestPacket(int16_t apiKey, int16_t apiVersion)
      : apiKey(apiKey), apiVersion(apiVersion) {}

  void Write(Buffer* buffer) {
    BasePacket::Write(buffer);

    //
    buffer->Write<int16_t>(apiKey);
    buffer->Write<int16_t>(apiVersion);
    buffer->Write<int32_t>(correlationId);
    buffer->Write<int16_t>(clientId);
  }

  std::size_t Size() { return BasePacket::Size() + 2 + 2 + 4 + 2; }

  int16_t apiKey;
  int16_t apiVersion;
  int32_t correlationId;
  int16_t clientId =
      -1;  // TODO: Currently "nulled" because we don't support client ids
};

struct ResponsePacket : public BasePacket {
  int32_t correlationId;
};

struct MetadataRequestPacket : public RequestPacket {
  MetadataRequestPacket(std::vector<std::string>& topics,
                        bool allowAutoTopicCreation,
                        bool includeClusterAuthorizedOperations,
                        bool includeTopicAuthorizedOperations)
      : RequestPacket(3, 8),
        topics(topics),
        allowAutoTopicCreation(allowAutoTopicCreation),
        includeClusterAuthorizedOperations(includeClusterAuthorizedOperations),
        includeTopicAuthorizedOperations(includeTopicAuthorizedOperations) {}

  void Write(Buffer* buffer) {
    RequestPacket::Write(buffer);

    // Write topics
    buffer->Write<int32_t>(topics.size());
    for (auto& topicName : topics) {
      buffer->WriteString(topicName);
    }

    // Write options
    buffer->WriteBoolean(allowAutoTopicCreation);
    buffer->WriteBoolean(includeClusterAuthorizedOperations);
    buffer->WriteBoolean(includeTopicAuthorizedOperations);

    // Write size
    packetSize = buffer->Size() - 4;
    buffer->Overwrite<int32_t>(packetSizePosition, packetSize);
  }

  std::size_t Size() {
    std::size_t packetSize = RequestPacket::Size() + 4 + 3;

    for (auto& topicName : topics) {
      packetSize += 2 + topicName.size();
    }

    return packetSize;
  }

  std::vector<std::string>& topics;
  bool allowAutoTopicCreation;
  bool includeClusterAuthorizedOperations;
  bool includeTopicAuthorizedOperations;
};
}  // namespace ahiv::kafka::protocol

#endif  // AHIV_KAFKA_CLIENT_PACKET_H
