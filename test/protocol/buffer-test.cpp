// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#include "ahiv/kafka/protocol/buffer.h"

#include "gtest/gtest.h"

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, CorrectEncodingOfTrueBoolean) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(1);
  buffer.WriteBoolean(true);

  EXPECT_EQ(buffer.Data()[0], 1);
  EXPECT_TRUE(buffer.ReadBoolean());
}

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, CorrectEncodingOfFalseBoolean) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(1);
  buffer.WriteBoolean(false);

  EXPECT_EQ(buffer.Data()[0], 0);
  EXPECT_FALSE(buffer.ReadBoolean());
}

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, CorrectShortEncoding) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(2);
  buffer.Write<int16_t>(255);

  EXPECT_EQ(buffer.Data()[0], 0);
  EXPECT_EQ(buffer.Data()[1], '\xFF');
  EXPECT_EQ(buffer.Read<int16_t>(), 255);
}

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, CorrectIntEncoding) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(4);
  auto pos = buffer.Write<int32_t>(128000);

  EXPECT_EQ(buffer.Data()[0], '\0');
  EXPECT_EQ(buffer.Data()[1], '\x01');
  EXPECT_EQ(buffer.Data()[2], '\xF4');
  EXPECT_EQ(buffer.Data()[3], '\0');
  EXPECT_EQ(buffer.Read<int32_t>(), 128000);
}

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, CorrectLongEncoding) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(8);
  buffer.Write<int64_t>(-849018880);

  EXPECT_EQ(buffer.Data()[0], '\xFF');
  EXPECT_EQ(buffer.Data()[1], '\xFF');
  EXPECT_EQ(buffer.Data()[2], '\xFF');
  EXPECT_EQ(buffer.Data()[3], '\xFF');
  EXPECT_EQ(buffer.Data()[4], '\xCD');
  EXPECT_EQ(buffer.Data()[5], 0x65);
  EXPECT_EQ(buffer.Data()[6], '\0');
  EXPECT_EQ(buffer.Data()[7], '\0');
  EXPECT_EQ(buffer.Read<int64_t>(), -849018880);
}

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, CorrectStringEncoding) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(6);
  buffer.WriteString("test");

  EXPECT_EQ(buffer.Data()[2], 't');
  EXPECT_EQ(buffer.Data()[3], 'e');
  EXPECT_EQ(buffer.Data()[4], 's');
  EXPECT_EQ(buffer.Data()[5], 't');
  EXPECT_EQ(buffer.ReadString(), "test");
}

// Test is the resolver works correctly when given a proper domain
TEST(BufferTest, OverwriteInt) {
  ahiv::kafka::protocol::Buffer buffer = ahiv::kafka::protocol::Buffer();
  buffer.EnsureAllocated(8);
  auto pos = buffer.Write<int32_t>(128000);
  buffer.Write<int32_t>(8);

  EXPECT_EQ(buffer.Data()[0], '\0');
  EXPECT_EQ(buffer.Data()[1], '\x01');
  EXPECT_EQ(buffer.Data()[2], '\xF4');
  EXPECT_EQ(buffer.Data()[3], '\0');
  EXPECT_EQ(buffer.Index(pos), 0);
  EXPECT_EQ(buffer.Read<int32_t>(), 128000);
  EXPECT_EQ(buffer.Read<int32_t>(), 8);

  buffer.Overwrite<int32_t>(pos, 96000);
  buffer.ResetReadPosition();
  EXPECT_EQ(buffer.Read<int32_t>(), 96000);
}