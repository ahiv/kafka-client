// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_UTIL_H_
#define AHIV_KAFKA_UTIL_H_

namespace ahiv::kafka {
template<class Value>
using ResponseCallback = std::function<void (Value&)>;

// DumpAsHex prints the input to stdout as a hex string.
static void DumpAsHex(const char* input, std::size_t length) {
  DumpAsHex(input, length, output);
}

// DumpAsHex prints the input to the given output stream as a hex string.
static void DumpAsHex(const char* input, std::size_t length, std::ostream output) {
  output << length << '\n\';
  for (int index = 0; index < length; index++) {
    fprintf(output, "%02X ", input[index]);
  }
  output << std::flush;
}

}

#endif // AHIV_KAFKA_UTIL_H_
