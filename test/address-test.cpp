// Copyright 2019 Ahiv Authors. All rights reserved. Use of this source  code
// is governed by a MIT-style license that can be found in the LICENSE file.

#include "ahiv/kafka/address.h"
#include "ahiv/kafka/error.h"
#include "ahiv/kafka/event.h"
#include "gtest/gtest.h"

// Test is the resolver works correctly when given a proper domain
TEST(AddressTest, EmitsResolvedOnCorrectDomain) {
  auto loop = uvw::Loop::getDefault();
  bool errorThrown = false;
  bool resolved = false;

  ahiv::kafka::Address address("localhost", "80");
  address.on<ahiv::kafka::ErrorEvent>(
      [&errorThrown](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
        if (errorEvent.error == ahiv::kafka::Error::DNSResolveFailed) {
          errorThrown = true;
        }
      });
  address.on<ahiv::kafka::ResolvedEvent>(
      [&resolved](const ahiv::kafka::ResolvedEvent& resolvedEvent,
                  auto& emitter) {
        EXPECT_FALSE(resolved);
        resolved = true;
      });
  address.Resolve(loop);
  loop->run();

  EXPECT_FALSE(errorThrown);
  EXPECT_TRUE(resolved);
}

// Test is the resolver works correctly and throws the correct error when given
// a non resolvable domain
TEST(AddressTest, EmitsErrorOnNonResolvableDomain) {
  auto loop = uvw::Loop::getDefault();
  bool errorThrown = false;
  bool resolved = false;

  ahiv::kafka::Address address("localhost-rofllol", "80");
  address.on<ahiv::kafka::ErrorEvent>(
      [&errorThrown](const ahiv::kafka::ErrorEvent& errorEvent, auto& emitter) {
        if (errorEvent.error == ahiv::kafka::Error::DNSResolveFailed) {
          errorThrown = true;
        }
      });
  address.on<ahiv::kafka::ResolvedEvent>(
      [&resolved](const ahiv::kafka::ResolvedEvent& resolvedEvent,
                  auto& emitter) {
        EXPECT_FALSE(resolved);
        resolved = true;
      });
  address.Resolve(loop);
  loop->run();

  EXPECT_TRUE(errorThrown);
  EXPECT_FALSE(resolved);
}