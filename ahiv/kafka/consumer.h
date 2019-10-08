// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_CONSUMER_H
#define AHIV_KAFKA_CLIENT_CONSUMER_H

#include "ahiv/kafka/connection.h"
#include "uvw.hpp"

namespace ahiv::kafka {
class Consumer : public Connection {
 public:
  Consumer(std::shared_ptr<uvw::Loop>& loop) : Connection(loop) {}
};
}  // namespace ahiv::kafka

#endif  // AHIV_KAFKA_CLIENT_CONSUMER_H
