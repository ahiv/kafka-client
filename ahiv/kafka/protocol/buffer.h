// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_PROTOOCOL_BUFFER_H
#define AHIV_KAFKA_PROTOOCOL_BUFFER_H

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>

#include "ahiv/kafka/protocol/endian.h"

namespace ahiv::kafka::protocol {
class Buffer {
 public:
  std::size_t Size() { return this->writePositionInBuffer; }

  template <typename T>
  std::size_t Write(T value) {
    std::size_t length = sizeof(T);
    T correctedValue = this->correctEndian<T>(value);
    char* byteRepresentation = (char*)&correctedValue;
    return this->WriteData(byteRepresentation, length);
  }

  template <typename T>
  T Read() {
    T value = 0;
    std::size_t length = sizeof(T);

    for (std::size_t currentBytePosition = 0; currentBytePosition < length;
         currentBytePosition++) {
      std::size_t shift = ((length - (currentBytePosition + 1)) * 8);
      value |= (T)(this->internalBuffer[this->readPositionInBuffer++] & 0xFF)
               << shift;
    }

    return value;
  }

  template <typename T, size_t size = sizeof(T)>
  void Overwrite(std::size_t pos, T value) {
    T correctedValue = this->correctEndian<T>(value);
    char* byteRepresentation = (char*)&correctedValue;

    for (std::size_t positionInData = 0; positionInData < size;
         positionInData++) {
      this->internalBuffer[pos + positionInData] = byteRepresentation[positionInData];
    }
  }

  std::size_t WriteBoolean(bool value) {
    char data[1];
    data[0] = value ? 1 : 0;
    return this->WriteData(data, 1);
  }

  bool ReadBoolean() {
    return this->internalBuffer[this->readPositionInBuffer++] == 1;
  }

  std::size_t WriteString(const std::string& value) {
    std::size_t startPosition = this->Write<int16_t>(value.size());
    this->WriteData(value.c_str(), value.size());
    return startPosition;
  }

  std::string ReadString() {
    auto stringLength = this->Read<int16_t>();
    if (stringLength == -1) {
        return "";
    }

    std::string readStringInto;

    for (int positionInString = 0; positionInString < stringLength;
         positionInString++) {
      readStringInto.push_back(
          this->internalBuffer[this->readPositionInBuffer++]);
    }

    return readStringInto;
  }

  std::unique_ptr<char[]> Data() { return std::move(this->internalBuffer); }

  void EnsureAllocated(std::size_t size) {
    this->internalBuffer = std::unique_ptr<char[]>(new char[size]);
  }

  void ResetReadPosition() { this->readPositionInBuffer = 0; }

  std::size_t WriteData(const char* data, std::size_t length) {
    std::size_t currentWritePosition = this->writePositionInBuffer;
    for (std::size_t positionInData = 0; positionInData < length;
         positionInData++) {
      this->internalBuffer[this->writePositionInBuffer++] =
          data[positionInData];
    }
    return currentWritePosition;
  }

  ~Buffer() {
      if (this->internalBuffer) {
          this->internalBuffer.release();
      }
  }

 private:
  template <typename T, size_t size = sizeof(T)>
  constexpr T correctEndian(T value) {
    switch (size) {
      case 8:
        return htobe64(value);
      case 4:
        return htobe32(value);
      case 2:
        return htobe16(value);
      default:
        return value;
    }
  }

  uint64_t readVarint(int8_t maxAmountOfBytes) {
    uint64_t num = 0;
    int shift = 0;

    do {
      if (maxAmountOfBytes-- == 0) return 0;

      num |= (uint64_t)(this->internalBuffer[this->readPositionInBuffer] & 0x7f)
             << shift;
      shift += 7;
    } while (this->internalBuffer[this->readPositionInBuffer++] & 0x80);

    return num;
  }

  void writeVarint(uint64_t number, int8_t maxAmountOfBytes) {
    int8_t current = 0;

    do {
      if (current >= maxAmountOfBytes) return;

      this->internalBuffer[this->writePositionInBuffer++] =
          (number & 0x7f) | (number > 0x7f ? 0x80 : 0);
      current++;
      number >>= 7;
    } while (number);
  }

  std::unique_ptr<char[]> internalBuffer;
  std::size_t writePositionInBuffer = 0;
  std::size_t readPositionInBuffer = 0;
};
}  // namespace ahiv::kafka::protocol

#endif  // AHIV_KAFKA_CLIENT_BUFFER_H
