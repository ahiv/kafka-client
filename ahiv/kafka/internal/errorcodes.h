// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_INTERNAL_ERRORCODES_H_
#define AHIV_KAFKA_INTERNAL_ERRORCODES_H_

namespace ahiv::kafka::internal {
enum class ErrorCode : int16_t {
  UNKNOWN_SERVER_ERROR = -1,
  NONE,
  OFFSET_OUT_OF_RANGE,
  CORRUPT_MESSAGE,
  UNKNOWN_TOPIC_OR_PARTITION,
  INVALID_FETCH_SIZE,
  LEADER_NOT_AVAILABLE,
  NOT_LEADER_FOR_PARTITION,
  REQUEST_TIMED_OUT,
  BROKER_NOT_AVAILABLE,
  REPLICA_NOT_AVAILABLE

};

bool IsErrorCodeRetryable(ErrorCode errorCode) {
  return errorCode == ErrorCode::CORRUPT_MESSAGE ||
      errorCode == ErrorCode::UNKNOWN_TOPIC_OR_PARTITION ||
      (errorCode >= ErrorCode::LEADER_NOT_AVAILABLE &&
          errorCode <= ErrorCode::REQUEST_TIMED_OUT);

}
}  // namespace ahiv::kafka::internal

#endif  // AHIV_KAFKA_INTERNAL_ERRORCODES_H_
