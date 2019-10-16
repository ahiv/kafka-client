// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_PROTOCOL_PACKET_METADATA_H
#define AHIV_KAFKA_PROTOCOL_PACKET_METADATA_H

#include <vector>
#include "ahiv/kafka/protocol/packet/base.h"

namespace ahiv::kafka::protocol::packet {

    // Metadata Request (Version: 8) => [topics] allow_auto_topic_creation include_cluster_authorized_operations include_topic_authorized_operations
    //  topics => name
    //    name => STRING
    //  allow_auto_topic_creation => BOOLEAN
    //  include_cluster_authorized_operations => BOOLEAN
    //  include_topic_authorized_operations => BOOLEAN
    struct MetadataRequestPacket : public RequestPacket {
        MetadataRequestPacket(std::vector<std::string> &topics,
                              bool allowAutoTopicCreation,
                              bool includeClusterAuthorizedOperations,
                              bool includeTopicAuthorizedOperations)
                : RequestPacket(3, 8),
                  topics(topics),
                  allowAutoTopicCreation(allowAutoTopicCreation),
                  includeClusterAuthorizedOperations(includeClusterAuthorizedOperations),
                  includeTopicAuthorizedOperations(includeTopicAuthorizedOperations) {}

        void Write(Buffer &buffer) {
            RequestPacket::Write(buffer);

            // Write topics
            buffer.Write<int32_t>(topics.size());
            for (auto &topicName : topics) {
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

            for (auto &topicName : topics) {
                packetSize += 2 + topicName.size();
            }

            return packetSize;
        }

        std::vector<std::string> &topics;
        bool allowAutoTopicCreation;
        bool includeClusterAuthorizedOperations;
        bool includeTopicAuthorizedOperations;
    };

    // Metadata Response (Version: 8) => throttle_time_ms [brokers] cluster_id controller_id [topics] cluster_authorized_operations
    //  throttle_time_ms => INT32
    //  brokers => node_id host port rack
    //    node_id => INT32
    //    host => STRING
    //    port => INT32
    //    rack => NULLABLE_STRING
    //  cluster_id => NULLABLE_STRING
    //  controller_id => INT32
    //  topics => error_code name is_internal [partitions] topic_authorized_operations
    //    error_code => INT16
    //    name => STRING
    //    is_internal => BOOLEAN
    //    partitions => error_code partition_index leader_id leader_epoch [replica_nodes] [isr_nodes] [offline_replicas]
    //      error_code => INT16
    //      partition_index => INT32
    //      leader_id => INT32
    //      leader_epoch => INT32
    //      replica_nodes => INT32
    //      isr_nodes => INT32
    //      offline_replicas => INT32
    //    topic_authorized_operations => INT32
    //  cluster_authorized_operations => INT32
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

    struct MetadataResponsePacket : public ResponsePacket {
        void Read(Buffer &buffer) {
            ResponsePacket::Read(buffer);

            throttledInMilliseconds = buffer.Read<int32_t>();
            auto amountOfBrokers = buffer.Read<int32_t>();
            brokers.reserve(amountOfBrokers);
            for (std::size_t currentBroker = 0; currentBroker < amountOfBrokers; currentBroker++) {
                BrokerNodeInformation information;
                information.Read(buffer);
                brokers.emplace_back(information);
            }
        }

        int32_t throttledInMilliseconds;
        std::vector<BrokerNodeInformation> brokers;

    };
}


#endif // AHIV_KAFKA_PROTOCOL_PACKET_METADATA_H
