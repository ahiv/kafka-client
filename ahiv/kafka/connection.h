// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_CONNECTION_H
#define AHIV_KAFKA_CLIENT_CONNECTION_H

#include <functional>
#include <optional>
#include <set>
#include <string>

#include "ahiv/kafka/connectionconfig.h"
#include "ahiv/kafka/error.h"
#include "ahiv/kafka/event.h"
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

    printf("Connecting to brokers requested\n");
    this->connectToServers(bootstrapServers);
  }

 protected:
  // Init a new connection with the given loop. All actions are processed via
  // the given loop
  Connection(std::shared_ptr<uvw::Loop>& loop) : loop(loop) {

  }

 private:
  // connectToServerViaTCP takes in the resolved connection config and connects
  // a TCP socket to the resolved IP:Port
  void connectToServerViaTCP(ConnectionConfig& connectionConfig) {
    std::shared_ptr<uvw::TCPHandle> tcpSocket =
        this->loop->resource<uvw::TCPHandle>();

    tcpSocket->once<uvw::WriteEvent>(
        [](const uvw::WriteEvent&, uvw::TCPHandle& handle) {
            printf("Wrote data\n");
            handle.close();
        });

    tcpSocket->once<uvw::ConnectEvent>(
        [](const uvw::ConnectEvent&, uvw::TCPHandle& handle) {
          printf("Connected to broker\n");
        });

    printf("Trying to connect\n");
    tcpSocket->connect(connectionConfig.address.resolvedAddress);
  }

  // connectToServer parses the server address and connects to the given IP or
  // hostname via TCP
  void connectToServer(const std::string& server) {
    ConnectionConfig config = ConnectionConfig::parseFromConnectionURL(server);
    config.address.on<ahiv::kafka::ErrorEvent>(
        [this](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
          this->publish(errorEvent);
        });
    config.address.on<ahiv::kafka::ResolvedEvent>(
        [this, &config](const ahiv::kafka::ResolvedEvent& resolvedEvent,
                        auto& emitter) {
          printf("Resolved IP");
          this->connectToServerViaTCP(config);
        });
    config.address.resolve(this->loop);
  }

  // connectToServers looks for all servers in the set and connects to valid
  // ones
  void connectToServers(const std::set<std::string>& servers) {
    for (const auto& server : servers) {
      if (this->canBeUsedForConnecting(server)) {
        printf("Connecting to server %s will be started\n", server.c_str());
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

  std::shared_ptr<uvw::Loop>& loop;
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_CONNECTION_H
