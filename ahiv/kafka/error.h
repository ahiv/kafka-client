// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_CLIENT_ERROR_H
#define AHIV_KAFKA_CLIENT_ERROR_H

namespace ahiv::kafka {
enum class Error {
  NoValidBootstrapServerGiven,
  DNSResolveFailed,
  TCPConnectionRefused,
  UnknownTCPError
};
}

#endif  // AHIV_KAFKA_CLIENT_ERROR_H
