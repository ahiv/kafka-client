// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_INTERNAL_TOPIC_H
#define AHIV_KAFKA_INTERNAL_TOPIC_H

#include "ahiv/kafka/internal/partition.h"

namespace ahiv::kafka::internal {
    class Topic {
    public:
        void askForMetadata(TCPConnection& connection) {

        }
    private:
        std::vector<Partition> partitions;
    };
}

#endif //AHIV_KAFKA_INTERNAL_TOPIC_H
