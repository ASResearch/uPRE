// Copyright (C) 2018 go-nebulas authors
//
// This file is part of the go-nebulas library.
//
// the go-nebulas library is free software: you can redistribute it and/or
// modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the go-nebulas library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the go-nebulas library.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "fs/memory_storage.h"
#include "fs/nt_item_simple_storage.h"
#include "fs/nt_items_storage.h"
#include <gtest/gtest.h>

typedef neb::fs::nt_items_storage<auth_param_storage_t> aps_t;
typedef neb::fs::nt_items_storage<nr_param_storage_t> nps_t;
typedef neb::fs::nt_items_storage<dip_param_storage_t> dps_t;

TEST(test_nt_items_storage, auth_param) {
  auth_param_storage_t aps;
  aps.set<p_start_block>(123);
  aps.set<p_version>(456);
  aps.set<p_raw_key>("789");
  aps.set<p_ir_key>("000");
  auto aps_str = aps.serialize_to_string();

  auth_param_storage_t ret;
  ret.deserialize_from_string(aps_str);
  EXPECT_EQ(ret.get<p_start_block>(), 123);
  EXPECT_EQ(ret.get<p_version>(), 456);
  EXPECT_EQ(ret.get<p_raw_key>(), "789");
  EXPECT_EQ(ret.get<p_ir_key>(), "000");
}

TEST(test_nt_items_storage, nr_param_t) {
  nr_param_storage_t nps;
  nps.set<p_start_block>(-1);
  nps.set<p_block_interval>(0);
  nps.set<p_version>(1);
  nps.set<p_raw_key>(std::string());
  nps.set<p_ir_key>("");
  auto nps_str = nps.serialize_to_string();

  nr_param_storage_t ret;
  ret.deserialize_from_string(nps_str);
  EXPECT_EQ(ret.get<p_start_block>(), -1);
  EXPECT_EQ(ret.get<p_block_interval>(), 0);
  EXPECT_EQ(ret.get<p_version>(), 1);
  EXPECT_EQ(ret.get<p_raw_key>(), std::string());
  EXPECT_EQ(ret.get<p_ir_key>(), "");
}

TEST(test_nt_items_storage, dip_param_storage_t) {
  dip_param_storage_t dps;
  dps.set<p_start_block>(0xffffffffffffffff);
  dps.set<p_block_interval>(0xffffffffffffffff);
  dps.set<p_version>(0xffffffffffffffff);
  dps.set<p_dip_reward_addr>(std::string());
  dps.set<p_dip_coinbase_addr>("");
  dps.set<p_raw_key>("0x123");
  dps.set<p_ir_key>("0x456");
  auto dps_str = dps.serialize_to_string();

  dip_param_storage_t ret;
  ret.deserialize_from_string(dps_str);
  EXPECT_EQ(ret.get<p_start_block>(), 0xffffffffffffffff);
  EXPECT_EQ(ret.get<p_block_interval>(), 0xffffffffffffffff);
  EXPECT_EQ(ret.get<p_version>(), 0xffffffffffffffff);
  EXPECT_EQ(ret.get<p_dip_reward_addr>(), std::string());
  EXPECT_EQ(ret.get<p_dip_coinbase_addr>(), "");
  EXPECT_EQ(ret.get<p_raw_key>(), "0x123");
  EXPECT_EQ(ret.get<p_ir_key>(), "0x456");
}

TEST(test_nt_items_storage, constructor) {
  const char *key_prefix = "auth_param_";
  const char *last_item_key = "auth_param_last_block";
  size_t block_trunk_size = 16;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto nis_ptr = std::make_unique<aps_t>(ms_ptr.get(), key_prefix,
                                         last_item_key, block_trunk_size);
  auto ret = nis_ptr->get_typed_items();
  EXPECT_TRUE(ret.empty());
}

TEST(test_nt_items_storage, auth_append_item) {
  const char *key_prefix = "auth_param_";
  const char *last_item_key = "auth_param_last_block";
  size_t block_trunk_size = 16;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto nis_ptr = std::make_unique<aps_t>(ms_ptr.get(), key_prefix,
                                         last_item_key, block_trunk_size);

  auth_param_storage_t aps_0;
  aps_0.set<p_start_block>(123);
  aps_0.set<p_version>(456);
  aps_0.set<p_raw_key>("789");
  aps_0.set<p_ir_key>("000");
  nis_ptr->append_item(aps_0);

  auth_param_storage_t aps_1;
  aps_1.set<p_start_block>(123);
  aps_1.set<p_version>(uint64_t());
  aps_1.set<p_raw_key>(std::string());
  aps_1.set<p_ir_key>("000");
  nis_ptr->append_item(aps_1);

  auth_param_storage_t aps_2;
  aps_2.set<p_start_block>(123);
  aps_2.set<p_version>(0xffffffffffffffff);
  aps_2.set<p_raw_key>(std::string());
  aps_2.set<p_ir_key>("");
  nis_ptr->append_item(aps_2);

  auto ret = nis_ptr->get_typed_items();
  EXPECT_EQ(ret.size(), 3);
  auto &first_ele = ret.front();
  EXPECT_EQ(first_ele.get<p_start_block>(), 123);
  EXPECT_EQ(first_ele.get<p_version>(), 456);
  EXPECT_EQ(first_ele.get<p_raw_key>(), "789");
  EXPECT_EQ(first_ele.get<p_ir_key>(), "000");
  auto &last_ele = ret.back();
  EXPECT_EQ(last_ele.get<p_start_block>(), 123);
  EXPECT_EQ(last_ele.get<p_version>(), 0xffffffffffffffff);
  EXPECT_EQ(last_ele.get<p_raw_key>(), "");
  EXPECT_EQ(last_ele.get<p_ir_key>(), "");

  for (size_t s = 0; s < 2 * block_trunk_size + 1; s++) {
    auth_param_storage_t aps;
    aps.set<p_start_block>(s);
    aps.set<p_version>(s * s);
    aps.set<p_raw_key>(std::to_string(2 * s + 1));
    aps.set<p_ir_key>(std::to_string(3 * s + 2));
    nis_ptr->append_item(aps);
    auto ret = nis_ptr->get_typed_items();
    EXPECT_EQ(ret.size(), s + 4);
    EXPECT_EQ(aps.get<p_start_block>(), s);
    EXPECT_EQ(aps.get<p_version>(), s * s);
    EXPECT_EQ(aps.get<p_raw_key>(), std::to_string(2 * s + 1));
    EXPECT_EQ(aps.get<p_ir_key>(), std::to_string(3 * s + 2));
  }
}

TEST(test_nt_items_storage, nr_append_item) {
  const char *key_prefix = "nr_param_";
  const char *last_item_key = "nr_param_last_block";
  size_t block_trunk_size = 32;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto nis_ptr = std::make_unique<nps_t>(ms_ptr.get(), key_prefix,
                                         last_item_key, block_trunk_size);

  nr_param_storage_t nps_0;
  nps_0.set<p_start_block>(123);
  nps_0.set<p_block_interval>(456);
  nps_0.set<p_version>(789);
  nps_0.set<p_raw_key>("abc");
  nps_0.set<p_ir_key>("xyz");
  nis_ptr->append_item(nps_0);

  nr_param_storage_t nps_1;
  nps_1.set<p_start_block>(123);
  nps_1.set<p_block_interval>(-1);
  nps_1.set<p_version>(uint64_t());
  nps_1.set<p_raw_key>(std::string());
  nps_1.set<p_ir_key>("xyz");
  nis_ptr->append_item(nps_1);

  nr_param_storage_t nps_2;
  nps_2.set<p_start_block>(-987);
  nps_2.set<p_block_interval>(-654);
  nps_2.set<p_version>(0xffffffffffffffff);
  nps_2.set<p_raw_key>("zzzzzzzzzzzzzzzz");
  nps_2.set<p_ir_key>("");
  nis_ptr->append_item(nps_2);

  auto ret = nis_ptr->get_typed_items();
  EXPECT_EQ(ret.size(), 3);
  auto &mid_ele = ret[1];
  EXPECT_EQ(mid_ele.get<p_start_block>(), 123);
  EXPECT_EQ(mid_ele.get<p_block_interval>(), -1);
  EXPECT_EQ(mid_ele.get<p_version>(), uint64_t());
  EXPECT_EQ(mid_ele.get<p_raw_key>(), "");
  EXPECT_EQ(mid_ele.get<p_ir_key>(), "xyz");
  auto &last_ele = ret.back();
  EXPECT_EQ(last_ele.get<p_start_block>(), -987);
  EXPECT_EQ(last_ele.get<p_block_interval>(), -654);
  EXPECT_EQ(last_ele.get<p_version>(), 0xffffffffffffffff);
  EXPECT_EQ(last_ele.get<p_raw_key>(), "zzzzzzzzzzzzzzzz");
  EXPECT_EQ(last_ele.get<p_ir_key>(), "");

  for (size_t s = 0; s < 3 * block_trunk_size + 2; s++) {
    nr_param_storage_t nps;
    nps.set<p_start_block>(s);
    nps.set<p_block_interval>(s - 1);
    nps.set<p_version>(s + 1);
    nps.set<p_raw_key>(std::to_string(3 * s - 1));
    nps.set<p_ir_key>(std::to_string(4 * s - 3));
    nis_ptr->append_item(nps);
    auto ret = nis_ptr->get_typed_items();
    EXPECT_EQ(ret.size(), s + 4);
    EXPECT_EQ(nps.get<p_start_block>(), s);
    EXPECT_EQ(nps.get<p_block_interval>(), s - 1);
    EXPECT_EQ(nps.get<p_version>(), s + 1);
    EXPECT_EQ(nps.get<p_raw_key>(), std::to_string(3 * s - 1));
    EXPECT_EQ(nps.get<p_ir_key>(), std::to_string(4 * s - 3));
  }
}

TEST(test_nt_items_storage, dip_append_item) {
  const char *key_prefix = "dip_param_";
  const char *last_item_key = "dip_param_last_block";
  size_t block_trunk_size = 32;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto nis_ptr = std::make_unique<dps_t>(ms_ptr.get(), key_prefix,
                                         last_item_key, block_trunk_size);

  dip_param_storage_t dps_0;
  dps_0.set<p_start_block>(123);
  dps_0.set<p_block_interval>(456);
  dps_0.set<p_version>(789);
  dps_0.set<p_raw_key>("abc");
  dps_0.set<p_ir_key>("xyz");
  dps_0.set<p_dip_reward_addr>("reward_addr");
  dps_0.set<p_dip_coinbase_addr>("coinbase_addr");
  nis_ptr->append_item(dps_0);

  dip_param_storage_t dps_1;
  dps_1.set<p_start_block>(123);
  dps_1.set<p_block_interval>(-1);
  dps_1.set<p_version>(uint64_t());
  dps_1.set<p_raw_key>(std::string());
  dps_1.set<p_ir_key>("xyz");
  dps_1.set<p_dip_reward_addr>("");
  dps_1.set<p_dip_coinbase_addr>(neb::byte_to_string(neb::bytes()));
  nis_ptr->append_item(dps_1);

  dip_param_storage_t dps_2;
  dps_2.set<p_start_block>(-987);
  dps_2.set<p_block_interval>(-654);
  dps_2.set<p_version>(0xffffffffffffffff);
  dps_2.set<p_raw_key>("zzzzzzzzzzzzzzzz");
  dps_2.set<p_ir_key>("");
  dps_2.set<p_dip_reward_addr>(neb::byte_to_string(neb::bytes({0xa, 0xb})));
  dps_2.set<p_dip_coinbase_addr>(neb::byte_to_string(neb::bytes()));
  nis_ptr->append_item(dps_2);

  auto ret = nis_ptr->get_typed_items();
  EXPECT_EQ(ret.size(), 3);
  auto &first_ele = ret.front();
  EXPECT_EQ(first_ele.get<p_start_block>(), 123);
  EXPECT_EQ(first_ele.get<p_block_interval>(), 456);
  EXPECT_EQ(first_ele.get<p_version>(), 789);
  EXPECT_EQ(first_ele.get<p_raw_key>(), "abc");
  EXPECT_EQ(first_ele.get<p_ir_key>(), "xyz");
  EXPECT_EQ(first_ele.get<p_dip_reward_addr>(), "reward_addr");
  EXPECT_EQ(first_ele.get<p_dip_coinbase_addr>(), "coinbase_addr");
  auto &mid_ele = ret[1];
  EXPECT_EQ(mid_ele.get<p_start_block>(), 123);
  EXPECT_EQ(mid_ele.get<p_block_interval>(), -1);
  EXPECT_EQ(mid_ele.get<p_version>(), uint64_t());
  EXPECT_EQ(mid_ele.get<p_raw_key>(), "");
  EXPECT_EQ(mid_ele.get<p_ir_key>(), "xyz");
  EXPECT_EQ(mid_ele.get<p_dip_reward_addr>(), "");
  EXPECT_EQ(mid_ele.get<p_dip_coinbase_addr>(),
            neb::byte_to_string(neb::bytes()));

  for (size_t s = 0; s < 3 * block_trunk_size + 1; s++) {
    dip_param_storage_t dps;
    dps.set<p_start_block>(s);
    dps.set<p_block_interval>(2 * s - 1);
    dps.set<p_version>(2 * s + 1);
    dps.set<p_raw_key>(std::to_string(5 * s - 1));
    dps.set<p_ir_key>(std::to_string(5 * s - 2));
    dps.set<p_dip_reward_addr>(std::to_string(6 * s + 1));
    dps.set<p_dip_coinbase_addr>(std::to_string(6 * s + 5));
    nis_ptr->append_item(dps);
    auto ret = nis_ptr->get_typed_items();
    EXPECT_EQ(ret.size(), s + 4);
    EXPECT_EQ(dps.get<p_start_block>(), s);
    EXPECT_EQ(dps.get<p_block_interval>(), 2 * s - 1);
    EXPECT_EQ(dps.get<p_version>(), 2 * s + 1);
    EXPECT_EQ(dps.get<p_raw_key>(), std::to_string(5 * s - 1));
    EXPECT_EQ(dps.get<p_ir_key>(), std::to_string(5 * s - 2));
    EXPECT_EQ(dps.get<p_dip_reward_addr>(), std::to_string(6 * s + 1));
    EXPECT_EQ(dps.get<p_dip_coinbase_addr>(), std::to_string(6 * s + 5));
  }
}
