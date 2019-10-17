// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_CONNECTION_H
#define AHIV_KAFKA_CLIENT_CONNECTION_H

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>

#include "ahiv/kafka/connectionconfig.h"
#include "ahiv/kafka/error.h"
#include "ahiv/kafka/event.h"
#include "ahiv/kafka/internal/tcpconnection.h"
#include "uvw.hpp"

namespace ahiv::kafka {
class Connection : public uvw::Emitter<Connection> {
 public:
  // Bootstrap connects to at least one of the servers given in the set. Every
  // server must have a protocol prefixed, currently plaintext:// is the only
  // supported connection protocol. If there is no server available an error
  // will be published to the error callback containing the
  // Error.NoValidBootstrapServerGiven code
  //
  // After connecting to a bootstrap server, the cluster metadata state is asked
  // for, to discover more brokers and connect to them async. Once a full
  // discovery (and connections to the brokers have been made) the connected
  // callback will be fired once. You can use the connection for further
  // configuration after that callback has been fired.
  void Bootstrap(const std::set<std::string>& bootstrapServers) {
    if (!this->canAtLeasOneBeUsedForConnecting(bootstrapServers)) {
      this->publish(ErrorEvent{
          .reason = "No valid server for bootstrapping has been found",
          .error = Error::NoValidBootstrapServerGiven});
    }

    this->connectToServers(bootstrapServers);
  }

  // On registers a listener for the given event via the E template type. This
  // listener gets called every time the event E is published on this instance
  template <typename E>
  void On(std::function<void(E&, Connection&)> listener) {
    this->on<E>(listener);
  }

  // Once registers a listener for the given event via the E template type. This
  // listener gets called on the first time the event E is published on this
  // instance
  template <typename E>
  void Once(std::function<void(E&, Connection&)> listener) {
    this->once<E>(listener);
  }

 protected:
  // Init a new connection with the given loop. All actions are processed via
  // the given loop
  Connection(std::shared_ptr<uvw::Loop>& loop) : loop(loop) {}

  // Send the given buffer to the first connection, if there is one
  void SendToFirstConnection(
      protocol::Buffer& buffer,
      const std::function<void(protocol::Buffer&)> responseCallback) {
    if (this->tcpHandles.size() > 0) {
      this->tcpHandles[0]->Write(buffer, responseCallback);
    }
  }

  void requestMetadataForTopics(std::vector<std::string>& wantedTopics,
                                bool autoCreate) {
    ahiv::kafka::protocol::packet::MetadataRequestPacket requestPacket =
        ahiv::kafka::protocol::packet::MetadataRequestPacket(
            wantedTopics, autoCreate, false, false);

    ahiv::kafka::protocol::Buffer buffer;
    buffer.EnsureAllocated(requestPacket.Size());
    requestPacket.Write(buffer);
    this->sendAndConsumeMetadataResponse(buffer);
  }

 private:
  void sendAndConsumeMetadataResponse(protocol::Buffer& buffer) {
    this->SendToFirstConnection(
        buffer, [this](protocol::Buffer& responseBuffer) {
          ahiv::kafka::protocol::packet::MetadataResponsePacket
              metadataResponsePacket;
          metadataResponsePacket.Read(responseBuffer);

          for (auto broker : metadataResponsePacket.brokers) {
            this->consumeFromMetadata(broker);
          }

          for (auto topic : metadataResponsePacket.topicInformation) {
            for (auto partition : topic.partitionInformation) {
                std::cout << "Partition index: " << partition.partitionIndex << "; Leader Node ID: " << partition.leaderId << std::endl << std::flush;
            }
          }
        });
  }

  void consumeFromMetadata(
      const protocol::packet::BrokerNodeInformation brokerNodeInformation) {
    for (auto tcpConnection : this->tcpHandles) {
      if (tcpConnection->ConsumeFromMetadata(brokerNodeInformation)) {
        this->tcpHandleByNodeId.insert(
            std::make_pair(brokerNodeInformation.nodeId, tcpConnection));
      }
    }
  }

  // connectToServerViaTCP takes in the resolved connection config and connects
  // a TCP socket to the resolved IP:Port
  void connectToServerViaTCP(
      const std::shared_ptr<ConnectionConfig>& connectionConfig) {
    auto tcpConnection =
        std::make_shared<internal::TCPConnection>(this->loop, connectionConfig);
    tcpConnection->On<ConnectedEvent>(
        [this](const ConnectedEvent& event, auto&) { this->publish(event); });
    tcpHandles.emplace_back(tcpConnection);
  }

  // connectToServer parses the server address and connects to the given IP or
  // hostname via TCP
  void connectToServer(const std::string& server) {
    auto config = ConnectionConfig::ParseFromConnectionURL(server);
    config->address->on<ahiv::kafka::ErrorEvent>(
        [this](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
          this->publish(errorEvent);
        });
    config->address->on<ahiv::kafka::ResolvedEvent>(
        [this, config](const ahiv::kafka::ResolvedEvent& resolvedEvent,
                       auto& emitter) { this->connectToServerViaTCP(config); });
    config->address->Resolve(this->loop);
  }

  // connectToServers looks for all servers in the set and connects to valid
  // ones
  void connectToServers(const std::set<std::string>& servers) {
    for (const auto& server : servers) {
      if (this->canBeUsedForConnecting(server)) {
        this->connectToServer(server);
      }
    }
  }

  // canBeUsedForConnecting checks if the given server address can be used to
  // connect to a broker
  bool canBeUsedForConnecting(const std::string& bootstrapServer) {
    return bootstrapServer.find("plaintext://") == 0;
  }

  // canAtLeasOneBeUsedForConnecting checks if the given set can be used for
  // connecting to at least one broker
  bool canAtLeasOneBeUsedForConnecting(
      const std::set<std::string>& bootstrapServers) {
    for (const auto& bootstrapServer : bootstrapServers) {
      if (this->canBeUsedForConnecting(bootstrapServer)) {
        return true;
      }
    }

    return false;
  }

  std::vector<std::shared_ptr<internal::TCPConnection>> tcpHandles;
  std::map<int32_t, std::shared_ptr<internal::TCPConnection>> tcpHandleByNodeId;
  std::shared_ptr<uvw::Loop>& loop;
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_CONNECTION_H
