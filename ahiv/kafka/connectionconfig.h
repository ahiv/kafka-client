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
  Address address;
  ConnectionType connectionType;

  static ConnectionConfig parseFromConnectionURL(const std::string& url) {
    std::string hostnameAndPort = url.substr(PlaintextLength);
    std::string::size_type positionOfColon = hostnameAndPort.find(":");

    if (positionOfColon == std::string::npos) {
      return ConnectionConfig{.address = Address(hostnameAndPort, "9092"),
                              .connectionType = ConnectionType::Plaintext};
    } else if (positionOfColon == hostnameAndPort.size() - 1) {
      return ConnectionConfig{
          .address = Address(
              hostnameAndPort.substr(0, hostnameAndPort.size() - 1), "9092"),
          .connectionType = ConnectionType::Plaintext};
    } else {
      std::string hostname = hostnameAndPort.substr(0, positionOfColon);
      std::string port =
          hostnameAndPort.substr(positionOfColon + 1, hostnameAndPort.size());

      return ConnectionConfig{.address = Address(hostname, port),
                              .connectionType = ConnectionType::Plaintext};
    }
  }
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_CONNECTIONCONFIG_H
