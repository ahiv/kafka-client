// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CONNECTION_H
#define AHIV_KAFKA_CONNECTION_H

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
#include "ahiv/kafka/internal/errorcodes.h"
#include "ahiv/kafka/internal/tcpconnection.h"
#include "ahiv/kafka/util.h"
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
  template <typename Message>
  void SendToFirstConnection(
      typename Message::Request&& request,
      ahiv::kafka::ResponseCallback<typename Message::Response> responseCallback) {
    if (this->tcpHandles.size() > 0) {
      this->tcpHandles[0]->Send<Message>(request, responseCallback);
    }
  }

  void requestMetadataForTopics(std::vector<std::string>& wantedTopics,
                                bool autoCreate) {
    this->requestMetadataForTopicsWithRetry(wantedTopics, autoCreate, 0);
  }

 private:
  void requestMetadataForTopicsWithRetry(std::vector<std::string>& wantedTopics,
                                         bool autoCreate, int8_t retries) {
    this->SendToFirstConnection<protocol::packet::MetadataPacket>(
        protocol::packet::MetadataRequestPacket(wantedTopics, autoCreate, false,
                                                false),
        [this, &wantedTopics, &retries,
         autoCreate](protocol::packet::MetadataResponsePacket& response) {
          for (const auto& broker : response.brokers) {
            auto tcpConnection = this->consumeFromMetadata(broker);
            if (tcpConnection != nullptr) {
              this->connectionInfoByNodeId.insert(std::make_pair(
                  broker.nodeId, tcpConnection->connectionConfig));
            }
          }

          bool retrying = false;
          for (const auto& topic : response.topicInformation) {
            if (topic.errorCode != 0) {
              if (internal::IsErrorCodeRetryable(
                      (internal::ErrorCode)topic.errorCode) &&
                  !retrying && retries < 25) {
                retrying = true;
                auto retryTimerHandle =
                    this->loop->resource<uvw::TimerHandle>();
                retryTimerHandle->on<uvw::TimerEvent>(
                    [this, &wantedTopics, autoCreate, &retries](const auto&,
                                                                auto& handle) {
                      this->requestMetadataForTopicsWithRetry(
                          wantedTopics, autoCreate, retries++);
                    });

                retryTimerHandle->start(
                    uvw::TimerHandle::Time{(retries + 1) * 100},
                    uvw::TimerHandle::Time{0});
              }
            } else {
              this->publish(
                  UpdateTopicInformationEvent{.topicInformation = topic});
            }
          }
        });
  }

  // consumeFromMetadata tells the tcp connections to grab their broker id if
  // they don't know them yet and stores the broker id in a map for lookup
  std::shared_ptr<internal::TCPConnection> consumeFromMetadata(
      const protocol::packet::BrokerNodeInformation& brokerNodeInformation) {
    for (const auto& tcpConnection : this->tcpHandles) {
      if (tcpConnection->ConsumeFromMetadata(brokerNodeInformation)) {
        this->tcpHandleByNodeId.insert(
            std::make_pair(brokerNodeInformation.nodeId, tcpConnection));
        return tcpConnection;
      }
    }

    return nullptr;
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
  std::map<int32_t, std::shared_ptr<ConnectionConfig>> connectionInfoByNodeId;
  std::shared_ptr<uvw::Loop>& loop;
  std::vector<std::string> wantedTopics;
  bool autoCreate;
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CONNECTION_H
