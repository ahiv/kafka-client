// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_EVENT_H
#define AHIV_KAFKA_CLIENT_EVENT_H

#include <string>

#include "ahiv/kafka/error.h"

namespace ahiv::kafka {
// ErrorEvent is fired when some component has ran into an issue. The reason is
// filled with a human readable and understandable error, the error property is
// filled with a enum value from Error.
struct ErrorEvent {
  const std::string& reason;
  const Error& error;
};

// ResolvedEvent is used by the Address class to notify the caller when it has
// resolved the IP of the hostname given
struct ResolvedEvent {};

struct ConnectedEvent {};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_EVENT_H
