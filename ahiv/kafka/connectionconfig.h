// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_CONNECTIONCONFIG_H
#define AHIV_KAFKA_CLIENT_CONNECTIONCONFIG_H

#include "ahiv/kafka/address.h"
#include "ahiv/kafka/event.h"
#include "uvw.hpp"

namespace ahiv::kafka {
const std::size_t PlaintextLength = 12;

enum class ConnectionType { Plaintext };

struct ConnectionConfig {
  Address* address = nullptr;
  ConnectionType connectionType;

  static std::shared_ptr<ConnectionConfig> parseFromConnectionURL(
      const std::string& url) {
    std::string hostnameAndPort = url.substr(PlaintextLength);
    std::string::size_type positionOfColon = hostnameAndPort.find(":");
    ConnectionConfig connectionConfig = ConnectionConfig {
      .connectionType = ConnectionType::Plaintext
    };

    if (positionOfColon == std::string::npos) {
      connectionConfig.address = new Address(hostnameAndPort, "9092");
    } else if (positionOfColon == hostnameAndPort.size() - 1) {
      connectionConfig.address = new Address(
          hostnameAndPort.substr(0, hostnameAndPort.size() - 1), "9092");
    } else {
      std::string hostname = hostnameAndPort.substr(0, positionOfColon);
      std::string port =
          hostnameAndPort.substr(positionOfColon + 1, hostnameAndPort.size());

      connectionConfig.address = new Address(hostname, port);
    }

    return std::make_shared<ConnectionConfig>(connectionConfig);
  }
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_CONNECTIONCONFIG_H
