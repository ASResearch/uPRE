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

#include "fs/items_storage.h"
#include "fs/memory_storage.h"
#include <gtest/gtest.h>

TEST(test_items_storage, ntpackage) {
  item_info_t ii;
  ii.set<p_item_content>(neb::string_to_byte("hahahah"));
  ii.set<p_item_count>(666);
  auto ii_str = ii.serialize_to_string();
  item_info_t ii_ret;
  ii_ret.deserialize_from_string(ii_str);
  EXPECT_EQ(ii_ret.get<p_item_content>(), neb::string_to_byte("hahahah"));
  EXPECT_EQ(ii_ret.get<p_item_count>(), 666);

  item_index_table_t iit;
  auto &ref_iit = iit.get<p_item_keys>();
  ref_iit.push_back(ii);
  auto iit_str = iit.serialize_to_string();
  item_index_table_t iit_ret;
  iit_ret.deserialize_from_string(iit_str);
  auto &ref_iit_ret = iit_ret.get<p_item_keys>();
  EXPECT_EQ(ref_iit_ret.size(), 1);
  EXPECT_EQ(ref_iit_ret[0].get<p_item_content>(),
            neb::string_to_byte("hahahah"));
  EXPECT_EQ(ref_iit_ret[0].get<p_item_count>(), 666);

  ii.set<p_item_content>(neb::string_to_byte("gg"));
  ii.set<p_item_count>(99);
  ref_iit_ret.push_back(ii);
  auto iit_ret_str = iit_ret.serialize_to_string();
  iit.deserialize_from_string(iit_ret_str);
  ref_iit = iit.get<p_item_keys>();
  EXPECT_EQ(ref_iit.size(), 2);
  EXPECT_EQ(ref_iit[0].get<p_item_content>(), neb::string_to_byte("hahahah"));
  EXPECT_EQ(ref_iit[0].get<p_item_count>(), 666);
  EXPECT_EQ(ref_iit[1].get<p_item_content>(), neb::string_to_byte("gg"));
  EXPECT_EQ(ref_iit[1].get<p_item_count>(), 99);

  item_contents_t ic;
  auto &ref_ic = ic.get<p_item_contents>();
  ref_ic.push_back(neb::string_to_byte("gogo"));
  auto ic_str = ic.serialize_to_string();
  item_contents_t ic_ret;
  ic_ret.deserialize_from_string(ic_str);
  auto &ref_ic_ret = ic_ret.get<p_item_contents>();
  EXPECT_EQ(ref_ic_ret.size(), 1);
  EXPECT_EQ(ref_ic_ret[0], neb::string_to_byte("gogo"));
  ref_ic_ret.push_back(neb::string_to_byte("nebulas"));
  auto ref_ic_str = ic_ret.serialize_to_string();
  ic.deserialize_from_string(ref_ic_str);
  ref_ic = ic.get<p_item_contents>();
  EXPECT_EQ(ref_ic.size(), 2);
  EXPECT_EQ(ref_ic[0], neb::string_to_byte("gogo"));
  EXPECT_EQ(ref_ic[1], neb::string_to_byte("nebulas"));
}

TEST(test_items_storage, constructor) {
  const char *key_prefix = "auth_param_";
  const char *last_item_key = "auth_param_last_block";
  size_t block_trunk_size = 32;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto isb_ptr = std::make_unique<neb::fs::internal::items_storage_base>(
      ms_ptr.get(), key_prefix, last_item_key, block_trunk_size);

  auto ret = isb_ptr->get_all_items();
  EXPECT_EQ(ret.empty(), true);
}

TEST(test_items_storage, append_item) {
  const char *key_prefix = "auth_param_";
  const char *last_item_key = "auth_param_last_block";
  size_t block_trunk_size = 32;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto isb_ptr = std::make_unique<neb::fs::internal::items_storage_base>(
      ms_ptr.get(), key_prefix, last_item_key, block_trunk_size);
  isb_ptr->append_item(neb::string_to_byte("hah"));
  auto ret = isb_ptr->get_all_items();
  EXPECT_EQ(ret.size(), 1);

  isb_ptr->append_item(neb::string_to_byte("xix"));
  ret = isb_ptr->get_all_items();
  EXPECT_EQ(ret.size(), 2);

  isb_ptr->append_item(neb::string_to_byte("heh"));
  ret = isb_ptr->get_all_items();
  EXPECT_EQ(ret.size(), 3);
}

TEST(test_items_storage, get_all_items) {
  const char *key_prefix = "auth_param_";
  const char *last_item_key = "auth_param_last_block";
  size_t block_trunk_size = 32;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto isb_ptr = std::make_unique<neb::fs::internal::items_storage_base>(
      ms_ptr.get(), key_prefix, last_item_key, block_trunk_size);

  for (size_t s = 0; s < 3 * block_trunk_size; s++) {
    isb_ptr->append_item(neb::number_to_byte<neb::bytes>(s));
    auto ret = isb_ptr->get_all_items();
    EXPECT_EQ(ret.size(), s + 1);
  }
}

