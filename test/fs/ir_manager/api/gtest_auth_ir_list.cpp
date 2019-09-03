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

#include "fs/ir_manager/api/auth_ir_list.h"
#include "fs/memory_storage.h"
#include "test/fs/ir_manager/gen_ir/gen_auth_ir.h"
#include <gtest/gtest.h>

TEST(test_auth_ir_list, constructor) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto ail_ptr = std::make_unique<neb::fs::auth_ir_list>(ms_ptr.get());

  EXPECT_TRUE(!ail_ptr->ir_exist(0));
  EXPECT_THROW(ail_ptr->get_raw_ir(0), neb::fs::storage_general_failure);
  EXPECT_THROW(ail_ptr->get_ir(0), neb::fs::storage_general_failure);
  EXPECT_THROW(ail_ptr->get_ir_brief_key_with_height(0), std::runtime_error);
  EXPECT_THROW(ail_ptr->find_ir_at_height(0), std::runtime_error);
  EXPECT_TRUE(ail_ptr->get_ir_names().empty());
  EXPECT_TRUE(ail_ptr->get_ir_versions().empty());
}

void expect_nbreir_eq(const nbre::NBREIR &expect, const nbre::NBREIR &actual) {
  EXPECT_EQ(expect.name(), actual.name());
  EXPECT_EQ(expect.version(), actual.version());
  EXPECT_EQ(expect.height(), actual.height());
  EXPECT_EQ(expect.ir(), actual.ir());
  EXPECT_EQ(expect.ir_type(), actual.ir_type());
}

TEST(test_auth_ir_list, simple) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto ail_ptr = std::make_unique<neb::fs::auth_ir_list>(ms_ptr.get());

  auto compiled_ir = gen_auth_ir_raw("auth", 1, 100);
  auto raw_ir = compiled_ir;
  gen_ir_compiled(compiled_ir);
  EXPECT_TRUE(!ail_ptr->ir_exist(1));
  ail_ptr->write_ir(raw_ir, compiled_ir);
  EXPECT_TRUE(ail_ptr->ir_exist(1));

  EXPECT_THROW(ail_ptr->get_raw_ir(0), neb::fs::storage_general_failure);
  auto ret_raw_ir = ail_ptr->get_raw_ir(1);
  expect_nbreir_eq(raw_ir, ret_raw_ir);

  EXPECT_THROW(ail_ptr->get_ir(0), neb::fs::storage_general_failure);
  auto ret_compiled_ir = ail_ptr->get_ir(1);
  expect_nbreir_eq(compiled_ir, ret_compiled_ir);

  EXPECT_THROW(ail_ptr->get_ir_brief_key_with_height(99), std::runtime_error);
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(100),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(101),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));

  EXPECT_THROW(ail_ptr->find_ir_at_height(99), std::runtime_error);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(100), compiled_ir);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(101), compiled_ir);

  auto ret_names = ail_ptr->get_ir_names();
  EXPECT_EQ(ret_names.size(), 1);
  EXPECT_EQ(ret_names[0], "auth");

  auto ret_versions = ail_ptr->get_ir_versions();
  EXPECT_EQ(ret_versions.size(), 1);
  EXPECT_EQ(ret_versions[0], 1);

  auto ret_auth_param = ail_ptr->get_ir_param(compiled_ir);
  EXPECT_EQ(ret_auth_param.get<p_start_block>(), 100);
  EXPECT_EQ(ret_auth_param.get<p_version>(), 1);
}

TEST(test_auth_ir_list, multi) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto ail_ptr = std::make_unique<neb::fs::auth_ir_list>(ms_ptr.get());

  auto compiled_ir_0 = gen_auth_ir_raw("auth", 1, 100);
  auto raw_ir_0 = compiled_ir_0;
  gen_ir_compiled(compiled_ir_0);
  ail_ptr->write_ir(raw_ir_0, compiled_ir_0);

  auto compiled_ir_1 = gen_auth_ir_raw("auth", 2, 150);
  auto raw_ir_1 = compiled_ir_1;
  gen_ir_compiled(compiled_ir_1);
  ail_ptr->write_ir(raw_ir_1, compiled_ir_1);

  EXPECT_TRUE(ail_ptr->ir_exist(1));
  EXPECT_TRUE(ail_ptr->ir_exist(2));

  expect_nbreir_eq(ail_ptr->get_raw_ir(1), raw_ir_0);
  expect_nbreir_eq(ail_ptr->get_raw_ir(2), raw_ir_1);
  expect_nbreir_eq(ail_ptr->get_ir(1), compiled_ir_0);
  expect_nbreir_eq(ail_ptr->get_ir(2), compiled_ir_1);

  EXPECT_THROW(ail_ptr->get_ir_brief_key_with_height(20), std::runtime_error);
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(100),
            neb::concate_name_version(raw_ir_0.name(), raw_ir_0.version()));
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(123),
            neb::concate_name_version(raw_ir_0.name(), raw_ir_0.version()));
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(149),
            neb::concate_name_version(raw_ir_0.name(), raw_ir_0.version()));
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(150),
            neb::concate_name_version(raw_ir_1.name(), raw_ir_1.version()));
  EXPECT_EQ(ail_ptr->get_ir_brief_key_with_height(200),
            neb::concate_name_version(raw_ir_1.name(), raw_ir_1.version()));

  EXPECT_THROW(ail_ptr->find_ir_at_height(80), std::runtime_error);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(100), compiled_ir_0);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(101), compiled_ir_0);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(144), compiled_ir_0);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(149), compiled_ir_0);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(150), compiled_ir_1);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(151), compiled_ir_1);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(180), compiled_ir_1);
  expect_nbreir_eq(ail_ptr->find_ir_at_height(250), compiled_ir_1);

  auto ret_names = ail_ptr->get_ir_names();
  EXPECT_EQ(ret_names.size(), 1);
  EXPECT_EQ(ret_names[0], "auth");

  auto ret_versions = ail_ptr->get_ir_versions();
  EXPECT_EQ(ret_versions.size(), 2);
  EXPECT_EQ(ret_versions[0], 1);
  EXPECT_EQ(ret_versions[1], 2);

  auto ret_auth_param = ail_ptr->get_ir_param(compiled_ir_0);
  EXPECT_EQ(ret_auth_param.get<p_start_block>(), 100);
  EXPECT_EQ(ret_auth_param.get<p_version>(), 1);
  ret_auth_param = ail_ptr->get_ir_param(compiled_ir_1);
  EXPECT_EQ(ret_auth_param.get<p_start_block>(), 150);
  EXPECT_EQ(ret_auth_param.get<p_version>(), 2);
}

