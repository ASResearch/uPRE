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

#include "fs/ir_manager/api/dip_ir_list.h"
#include "fs/memory_storage.h"
#include "test/fs/ir_manager/gen_ir/gen_auth_ir.h"
#include "test/fs/ir_manager/gen_ir/gen_dip_ir.h"
#include "test/gtest_common.h"
#include <gtest/gtest.h>

TEST(test_dip_ir_list, constructor) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto dil_ptr = std::make_unique<neb::fs::dip_ir_list>(ms_ptr.get());

  EXPECT_TRUE(!dil_ptr->ir_exist(0));
  EXPECT_THROW(dil_ptr->get_raw_ir(0), neb::fs::storage_general_failure);
  EXPECT_THROW(dil_ptr->get_ir(0), neb::fs::storage_general_failure);
  EXPECT_THROW(dil_ptr->get_ir_brief_key_with_height(0), std::runtime_error);
  EXPECT_THROW(dil_ptr->find_ir_at_height(0), std::runtime_error);
  EXPECT_TRUE(dil_ptr->get_ir_names().empty());
  EXPECT_TRUE(dil_ptr->get_ir_versions().empty());
}

TEST(test_dip_ir_list, simple) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto dil_ptr = std::make_unique<neb::fs::dip_ir_list>(ms_ptr.get());

  auto compiled_ir = gen_dip_ir_raw("dip", 1, 300);
  auto raw_ir = compiled_ir;
  gen_ir_compiled(compiled_ir);
  EXPECT_TRUE(!dil_ptr->ir_exist(1));
  dil_ptr->write_ir(raw_ir, compiled_ir);
  EXPECT_TRUE(dil_ptr->ir_exist(1));

  EXPECT_THROW(dil_ptr->get_raw_ir(0), neb::fs::storage_general_failure);
  auto ret_raw_ir = dil_ptr->get_raw_ir(1);
  expect_nbreir_eq(raw_ir, ret_raw_ir);

  EXPECT_THROW(dil_ptr->get_ir(0), neb::fs::storage_general_failure);
  auto ret_compiled_ir = dil_ptr->get_ir(1);
  expect_nbreir_eq(compiled_ir, ret_compiled_ir);

  EXPECT_THROW(dil_ptr->get_ir_brief_key_with_height(299), std::runtime_error);
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(300),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(301),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));

  EXPECT_THROW(dil_ptr->find_ir_at_height(299), std::runtime_error);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(300), compiled_ir);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(301), compiled_ir);

  auto ret_names = dil_ptr->get_ir_names();
  EXPECT_EQ(ret_names.size(), 1);
  EXPECT_EQ(ret_names[0], "dip");

  auto ret_versions = dil_ptr->get_ir_versions();
  EXPECT_EQ(ret_versions.size(), 1);
  EXPECT_EQ(ret_versions[0], 1);

  auto ret_dip_param = dil_ptr->get_ir_param(compiled_ir);
  EXPECT_EQ(ret_dip_param.get<p_start_block>(), 300);
  EXPECT_EQ(ret_dip_param.get<p_version>(), 1);

  auto compiled_ir_1 = gen_dip_ir_raw("dip", 2, 1500);
  auto raw_ir_1 = compiled_ir_1;
  gen_ir_compiled(compiled_ir_1);
  EXPECT_TRUE(!dil_ptr->ir_exist(2));
  dil_ptr->write_ir(raw_ir_1, compiled_ir_1);
  EXPECT_TRUE(dil_ptr->ir_exist(2));

  EXPECT_THROW(dil_ptr->get_raw_ir(0), neb::fs::storage_general_failure);
  auto ret_raw_ir_1 = dil_ptr->get_raw_ir(2);
  expect_nbreir_eq(raw_ir_1, ret_raw_ir_1);

  EXPECT_THROW(dil_ptr->get_ir(0), neb::fs::storage_general_failure);
  auto ret_compiled_ir_1 = dil_ptr->get_ir(2);
  expect_nbreir_eq(compiled_ir_1, ret_compiled_ir_1);

  EXPECT_THROW(dil_ptr->get_ir_brief_key_with_height(299), std::runtime_error);
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(300),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(301),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(1499),
            neb::concate_name_version(raw_ir.name(), raw_ir.version()));
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(1500),
            neb::concate_name_version(raw_ir_1.name(), raw_ir_1.version()));
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(1501),
            neb::concate_name_version(raw_ir_1.name(), raw_ir_1.version()));
  EXPECT_EQ(dil_ptr->get_ir_brief_key_with_height(2000),
            neb::concate_name_version(raw_ir_1.name(), raw_ir_1.version()));

  EXPECT_THROW(dil_ptr->find_ir_at_height(299), std::runtime_error);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(300), compiled_ir);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(301), compiled_ir);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(1499), compiled_ir);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(1500), compiled_ir_1);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(1501), compiled_ir_1);
  expect_nbreir_eq(dil_ptr->find_ir_at_height(2000), compiled_ir_1);

  ret_names = dil_ptr->get_ir_names();
  EXPECT_EQ(ret_names.size(), 1);
  EXPECT_EQ(ret_names[0], "dip");

  ret_versions = dil_ptr->get_ir_versions();
  EXPECT_EQ(ret_versions.size(), 2);
  EXPECT_EQ(ret_versions[0], 1);
  EXPECT_EQ(ret_versions[1], 2);

  ret_dip_param = dil_ptr->get_ir_param(compiled_ir);
  EXPECT_EQ(ret_dip_param.get<p_start_block>(), 300);
  EXPECT_EQ(ret_dip_param.get<p_version>(), 1);
  ret_dip_param = dil_ptr->get_ir_param(compiled_ir_1);
  EXPECT_EQ(ret_dip_param.get<p_start_block>(), 1500);
  EXPECT_EQ(ret_dip_param.get<p_version>(), 2);
}
