// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_TCPCONNECTION_H
#define AHIV_KAFKA_CLIENT_TCPCONNECTION_H

#include <atomic>
#include <queue>

#include "ahiv/kafka/connectionconfig.h"
#include "ahiv/kafka/protocol/buffer.h"
#include "ahiv/kafka/protocol/packet.h"
#include "ahiv/kafka/util.h"
#include "uvw.hpp"

namespace ahiv::kafka::internal {
struct ResponseCorrelationCallback {
  int32_t correlationId;
  const std::function<void(protocol::Buffer&)> responseCallback;
};

class TCPConnection : public uvw::Emitter<TCPConnection> {
 public:
  TCPConnection(const std::shared_ptr<uvw::Loop>& loop,
                const std::shared_ptr<ConnectionConfig>& connectionConfig) {
    this->handle = loop->resource<uvw::TCPHandle>();

    this->handle->on<uvw::ErrorEvent>(
        [this](const uvw::ErrorEvent& errorEvent, auto&) {
          const char* errorName = errorEvent.name();
          if (errorName == "ECONNREFUSED") {
            this->publish(
                ErrorEvent{.reason = std::string("Could not connect to IP ")
                                         .append(errorEvent.what()),
                           .error = Error::TCPConnectionRefused});
          } else {
            this->publish(
                ErrorEvent{.reason = std::string("Got unknown TCP error: ")
                                         .append(errorEvent.what()),
                           .error = Error::UnknownTCPError});
          }
        });

    this->handle->once<uvw::ConnectEvent>(
        [this](const uvw::ConnectEvent&, uvw::TCPHandle& handle) {
          handle.read();
          this->publish(ConnectedEvent{});
        });

    this->handle->on<uvw::DataEvent>(
        [this](const uvw::DataEvent& event, uvw::TCPHandle&) {
          ahiv::kafka::protocol::Buffer buffer;
          buffer.EnsureAllocated(event.length);
          buffer.WriteData(event.data.get(), event.length);
          int32_t payloadLength = buffer.Read<int32_t>();
          int32_t correlationId = buffer.Read<int32_t>();

          ResponseCorrelationCallback callback = this->responseCallbacks.front();
          if (callback.correlationId == correlationId) {
              this->responseCallbacks.pop();
              buffer.ResetReadPosition();
              callback.responseCallback(buffer);
          } else {
              DumpAsHex(event.data.get(), event.length);
          }
        });

    this->handle->connect(*connectionConfig->address->resolvedAddress);
  }

  // On registers a listener for the given event via the E template type. This
  // listener gets called every time the event E is published on this instance
  template <typename E>
  void On(std::function<void(E&, TCPConnection&)> listener) {
    this->on<E>(listener);
  }

  // Once registers a listener for the given event via the E template type. This
  // listener gets called on the first time the event E is published on this
  // instance
  template <typename E>
  void Once(std::function<void(E&, TCPConnection&)> listener) {
    this->once<E>(listener);
  }

  // Write data into the stream
  void Write(protocol::Buffer& buffer,
             const std::function<void(protocol::Buffer&)> responseCallback) {
    int32_t correlationId = this->idCounter.fetch_add(1);
    this->responseCallbacks.emplace(ResponseCorrelationCallback{
      correlationId : correlationId,
      responseCallback : responseCallback
    });

    buffer.Overwrite<int32_t>(8, correlationId);
    handle->write(buffer.Data(), buffer.Size());
  }

 private:
  int32_t brokerId;
  std::shared_ptr<uvw::TCPHandle> handle;
  std::queue<ResponseCorrelationCallback> responseCallbacks;
  std::atomic<int32_t> idCounter;
};  // namespace ahiv::kafka::internal
}  // namespace ahiv::kafka::internal

#endif  // AHIV_KAFKA_CLIENT_TCPCONNECTION_H
