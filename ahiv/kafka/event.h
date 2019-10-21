// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_EVENT_H_
#define AHIV_KAFKA_EVENT_H_

#include <string>

#include "ahiv/kafka/error.h"
#include "ahiv/kafka/protocol/packet/metadata.h"

namespace ahiv::kafka {
// ErrorEvent is fired when some component has run into an issue.
struct ErrorEvent {
  // Reason captures the error cause in a humanly readable form.
  std::string Reason;
  // Error is the kind of error that occurred.
  Error Error;
};

// ResolvedEvent is used by the Address class to notify the caller when it has
// resolved the IP of the given hostname.
struct ResolvedEvent {};

struct ConnectedEvent {};

// UpdateTopicInformationEvent is fired when metadata changes have been detected
// for a topic. The updated metadata is part of this event.
struct UpdateTopicInformationEvent {
  protocol::packet::TopicInformation topicInformation;
};

}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_EVENT_H_
