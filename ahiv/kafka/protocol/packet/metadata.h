// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_PROTOCOL_PACKET_METADATA_H
#define AHIV_KAFKA_PROTOCOL_PACKET_METADATA_H

#include <vector>

#include "ahiv/kafka/protocol/packet/base.h"

namespace ahiv::kafka::protocol::packet {

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

  void Write(Buffer& buffer) {
    RequestPacket::Write(buffer);

    // Write topics
    buffer.Write<int32_t>(topics.size());
    for (auto& topicName : topics) {
      buffer.WriteString(topicName);
    }

    // Write options
    buffer.WriteBoolean(allowAutoTopicCreation);
    buffer.WriteBoolean(includeClusterAuthorizedOperations);
    buffer.WriteBoolean(includeTopicAuthorizedOperations);

    // Write size
    packetSize = buffer.Size() - 4;
    buffer.Overwrite<int32_t>(packetSizePosition, packetSize);
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

struct BrokerNodeInformation {
  int32_t nodeId;
  std::string host;
  int32_t port;
  std::string rack;

  void Read(Buffer& buffer) {
    nodeId = buffer.Read<int32_t>();
    host = buffer.ReadString();
    port = buffer.Read<int32_t>();
    rack = buffer.ReadString();
  }
};

struct PartitionInformation {
  void Read(Buffer& buffer) {
    errorCode = buffer.Read<int16_t>();
    partitionIndex = buffer.Read<int32_t>();
    leaderId = buffer.Read<int32_t>();
    leaderEpoch = buffer.Read<int32_t>();

    int32_t amountOfReplicas = buffer.Read<int32_t>();
    replicas.reserve(amountOfReplicas);
    for (std::size_t currentReplica = 0; currentReplica < amountOfReplicas;
         currentReplica++) {
      replicas.emplace_back(buffer.Read<int32_t>());
    }

    int32_t amountOfIsr = buffer.Read<int32_t>();
    isr.reserve(amountOfIsr);
    for (std::size_t currentISR = 0; currentISR < amountOfIsr; currentISR++) {
      isr.emplace_back(buffer.Read<int32_t>());
    }

    int32_t amountOfOfflineReplicas = buffer.Read<int32_t>();
    offlineReplicas.reserve(amountOfOfflineReplicas);
    for (std::size_t currentOfflineReplica = 0;
         currentOfflineReplica < amountOfOfflineReplicas;
         currentOfflineReplica++) {
      offlineReplicas.emplace_back(buffer.Read<int32_t>());
    }
  }

  int16_t errorCode;
  int32_t partitionIndex;
  int32_t leaderId;
  int32_t leaderEpoch;
  std::vector<int32_t> replicas;
  std::vector<int32_t> isr;
  std::vector<int32_t> offlineReplicas;
};

struct TopicInformation {
  void Read(Buffer& buffer) {
    errorCode = buffer.Read<int16_t>();
    name = buffer.ReadString();
    isInternal = buffer.ReadBoolean();

    int32_t amountOfPartitions = buffer.Read<int32_t>();
    partitionInformation.reserve(amountOfPartitions);
    for (std::size_t currentPartition = 0;
         currentPartition < amountOfPartitions; currentPartition++) {
      PartitionInformation partitionInfo;
      partitionInfo.Read(buffer);
      partitionInformation.emplace_back(partitionInfo);
    }

    topicAuthorizedOperations = buffer.Read<int32_t>();
  }

  int16_t errorCode;
  std::string name;
  bool isInternal;
  std::vector<PartitionInformation> partitionInformation;
  int32_t topicAuthorizedOperations;
};

struct MetadataResponsePacket : public ResponsePacket {
  void Read(Buffer& buffer) {
    ResponsePacket::Read(buffer);

    throttledInMilliseconds = buffer.Read<int32_t>();
    auto amountOfBrokers = buffer.Read<int32_t>();
    brokers.reserve(amountOfBrokers);
    for (std::size_t currentBroker = 0; currentBroker < amountOfBrokers;
         currentBroker++) {
      BrokerNodeInformation information;
      information.Read(buffer);
      brokers.emplace_back(information);
    }

    clusterId = buffer.ReadString();
    controllerId = buffer.Read<int32_t>();

    int32_t amountOfTopics = buffer.Read<int32_t>();
    topicInformation.reserve(amountOfTopics);
    for (std::size_t currentTopic = 0; currentTopic < amountOfTopics;
         currentTopic++) {
      TopicInformation topicInfo;
      topicInfo.Read(buffer);
      topicInformation.emplace_back(topicInfo);
    }

    clusterAuthorizedOperations = buffer.Read<int32_t>();
  }

  int32_t throttledInMilliseconds;
  std::vector<BrokerNodeInformation> brokers;
  std::string clusterId;
  int32_t controllerId;
  std::vector<TopicInformation> topicInformation;
  int32_t clusterAuthorizedOperations;
};

struct MetadataPacket {
  using Request = MetadataRequestPacket;
  using Response = MetadataResponsePacket;
};
}  // namespace ahiv::kafka::protocol::packet

#endif  // AHIV_KAFKA_PROTOCOL_PACKET_METADATA_H
