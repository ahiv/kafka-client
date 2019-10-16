// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_PARTITION_H
#define AHIV_KAFKA_CLIENT_PARTITION_H

namespace ahiv::kafka::internal {
    class Partition {

    private:
        int64_t offset;
    };
}


#endif //AHIV_KAFKA_CLIENT_PARTITION_H
