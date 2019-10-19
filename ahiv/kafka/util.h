// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_UTIL_H
#define AHIV_KAFKA_UTIL_H

namespace ahiv::kafka {
    static void DumpAsHex(const char* data, std::size_t length) {
        std::cout << length << std::endl << std::flush;
        for (int j = 0; j < length; j++) printf("%02X ", data[j]);
        std::cout << std::flush;
    }
}


#endif //AHIV_KAFKA_UTIL_H
