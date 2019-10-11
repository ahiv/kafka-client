// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#include "ahiv/kafka/consumer.h"
#include "ahiv/kafka/error.h"
#include "ahiv/kafka/event.h"
#include "gtest/gtest.h"

// Test if the connection throws the correct error when given only invalid
// servers to choose from
TEST(ConnectionTest, EmitsErrorWhenNoValidBootstrapIsGiven) {
  auto loop = uvw::Loop::getDefault();
  bool errorThrown = false;

  ahiv::kafka::Consumer consumer(loop);
  consumer.On<ahiv::kafka::ErrorEvent>(
      [&errorThrown](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
        if (errorEvent.error ==
            ahiv::kafka::Error::NoValidBootstrapServerGiven) {
          errorThrown = true;
        }
      });
  consumer.Bootstrap({"plaintext1://localhost:9092"});

  EXPECT_TRUE(errorThrown);
}

// Test if the connection throws no error when given a valid server address
TEST(ConnectionTest, EmitsNoErrorWhenValidBootstrapIsGiven) {
  auto loop = uvw::Loop::getDefault();
  bool errorThrown = false;

  ahiv::kafka::Consumer consumer(loop);
  consumer.On<ahiv::kafka::ErrorEvent>(
      [&errorThrown](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
        if (errorEvent.error ==
            ahiv::kafka::Error::NoValidBootstrapServerGiven) {
          errorThrown = true;
        }
      });
  consumer.Bootstrap({"plaintext://localhost:9092"});

  EXPECT_FALSE(errorThrown);
}
