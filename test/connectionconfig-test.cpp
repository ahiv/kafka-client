// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#include "ahiv/kafka/connectionconfig.h"

#include "gtest/gtest.h"

// Test if the parser correctly parses valid plaintext urls
TEST(ConnectionConfigTest, ParseSuccessWithCorrectURL) {
  auto connectionConfig = ahiv::kafka::ConnectionConfig::parseFromConnectionURL(
      "plaintext://localhost:9093");
  EXPECT_EQ(connectionConfig.connectionType,
            ahiv::kafka::ConnectionType::Plaintext);
  EXPECT_EQ(connectionConfig.address.hostname, "localhost");
  EXPECT_EQ(connectionConfig.address.port, "9093");
}

TEST(ConnectionConfigTest, ParseSuccessWithMissingPortURL) {
  auto connectionConfig = ahiv::kafka::ConnectionConfig::parseFromConnectionURL(
      "plaintext://localhost:");
  EXPECT_EQ(connectionConfig.connectionType,
            ahiv::kafka::ConnectionType::Plaintext);
  EXPECT_EQ(connectionConfig.address.hostname, "localhost");
  EXPECT_EQ(connectionConfig.address.port, "9092");
}

TEST(ConnectionConfigTest, ParseSuccessWithDefaultPortURL) {
  auto connectionConfig = ahiv::kafka::ConnectionConfig::parseFromConnectionURL(
      "plaintext://localhost");
  EXPECT_EQ(connectionConfig.connectionType,
            ahiv::kafka::ConnectionType::Plaintext);
  EXPECT_EQ(connectionConfig.address.hostname, "localhost");
  EXPECT_EQ(connectionConfig.address.port, "9092");
}

TEST(ConnectionConfigTest, ResolveHostWithSuccess) {
    auto loop = uvw::Loop::getDefault();
    auto connectionConfig = ahiv::kafka::ConnectionConfig::parseFromConnectionURL(
            "plaintext://localhost");
    EXPECT_EQ(connectionConfig.connectionType,
    ahiv::kafka::ConnectionType::Plaintext);
    EXPECT_EQ(connectionConfig.address.hostname, "localhost");
    EXPECT_EQ(connectionConfig.address.port, "9092");

    connectionConfig.address.resolve(loop);
}
