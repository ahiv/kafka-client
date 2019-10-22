// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_UTIL_H_
#define AHIV_KAFKA_UTIL_H_

#include <iostream>

namespace ahiv::kafka {

// DumpAsHex prints the input to stdout as a hex string.
static void DumpAsHex(const char* input, std::size_t length) {
  DumpAsHex(input, length, std::cout);
}

// DumpAsHex prints the input to the given output stream as a hex string.
static void DumpAsHex(const char* input, std::size_t length, std::ostream output) {
  output << length << '\n\';
  for (int index = 0; index < length; index++) {
    fprintf(output, "%02X ", input[index]);
  }
  output << std::flush;
}

} // namespace ahiv::kafka

#endif // AHIV_KAFKA_UTIL_H_
