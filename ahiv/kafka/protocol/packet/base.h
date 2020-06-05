// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#ifndef AHIV_KAFKA_PROTOCOL_PACKET_BASE_H
#define AHIV_KAFKA_PROTOCOL_PACKET_BASE_H

#include "ahiv/kafka/protocol/buffer.h"

namespace ahiv::kafka::protocol::packet {
    struct BasePacket {
        std::size_t packetSizePosition{};
        int32_t packetSize{};

        virtual void Write(Buffer& buffer) {
            packetSizePosition = buffer.Write<int32_t>(0);
        }

        virtual void Read(Buffer& buffer) {
            packetSize = buffer.Read<int32_t>();
        }

        virtual std::size_t Size() { return 4; }
    };

    struct RequestPacket : public BasePacket {
        RequestPacket(int16_t apiKey, int16_t apiVersion)
                : apiKey(apiKey), apiVersion(apiVersion) {}

        void Write(Buffer& buffer) override {
            BasePacket::Write(buffer);

            //
            buffer.Write<int16_t>(apiKey);
            buffer.Write<int16_t>(apiVersion);
            buffer.Write<int32_t>(correlationId);
            buffer.Write<int16_t>(clientId);
        }

        std::size_t Size() { return BasePacket::Size() + 2 + 2 + 4 + 2; }

        int16_t apiKey;
        int16_t apiVersion;
        int32_t correlationId{};
        int16_t clientId =
                -1;  // TODO: Currently "nulled" because we don't support client ids
    };

    struct ResponsePacket : public BasePacket {
        int32_t correlationId{};

        void Read(Buffer& buffer) override {
            BasePacket::Read(buffer);
            correlationId = buffer.Read<int32_t>();
        }
    };
}


#endif // AHIV_KAFKA_PROTOCOL_PACKET_BASE_H
