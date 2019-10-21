// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_ADDRESS_H
#define AHIV_KAFKA_ADDRESS_H

#include <memory>
#include <string>
#include <iostream>

#include "ahiv/kafka/event.h"
#include "uvw.hpp"

namespace ahiv::kafka {
class Address : public uvw::Emitter<Address> {
 public:
  Address(const std::string hostname, const std::string port)
      : hostname(hostname), port(port) {}

  void Resolve(const std::shared_ptr<uvw::Loop>& loop) {
    auto request = loop->resource<uvw::GetAddrInfoReq>();
    request->on<uvw::ErrorEvent>(
        [this](const uvw::ErrorEvent& errorEvent, auto&) {
          this->publish(ErrorEvent{
              .Reason = std::string("Could not resolve IP").append(errorEvent.what()),
              .Error = Error::DNSResolveFailed});
        });

    request->on<uvw::AddrInfoEvent>(
        [this](const uvw::AddrInfoEvent& addrInfoEvent, auto&) {
          this->resolvedAddress = addrInfoEvent.data->ai_addr;
          this->publish(ResolvedEvent{});
        });

    request->addrInfo(this->hostname, this->port);
  }

  sockaddr* resolvedAddress;
  const std::string hostname;
  const std::string port;
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_ADDRESS_H
