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

#include "common/configuration.h"
#include "fs/blockchain.h"
#include "fs/ir_manager/api/ir_list.h"
#include "fs/ir_manager/ir_processor.h"
#include "fs/memory_storage.h"
#include "fs/util.h"
#include "runtime/auth/auth_handler.h"
#include "runtime/auth/auth_table.h"
#include "test/fs/ir_manager/gen_block_transactions/gen_without_missed_blocks.h"
#include "test/gtest_common.h"
#include "util/persistent_flag.h"
#include "util/persistent_type.h"
#include <gtest/gtest.h>

TEST(test_ir_processor, constructor) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto bc_ptr = std::make_unique<neb::fs::blockchain>(ms_ptr.get());
  auto ip_ptr =
      std::make_unique<neb::fs::ir_processor>(ms_ptr.get(), bc_ptr.get());

  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_version("auth", 1));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("auth", 100));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_height("auth", 100));
  EXPECT_TRUE(ip_ptr->get_ir_names().empty());
  EXPECT_TRUE(ip_ptr->get_ir_versions("auth").empty());
  EXPECT_EQ(ms_ptr.get(), ip_ptr->storage());
  EXPECT_EQ(bc_ptr.get(), ip_ptr->blockchain());
}

TEST(test_ir_processor, gen_transactions) {
  neb::util::wakeable_queue<std::shared_ptr<nbre_ir_transactions_req>> q;
  std::vector<std::pair<protocol_type, int32_t>> v({{protocol_auth, 1}});
  neb::block_height_t height = 500;
  gen_transactions(height, v, q);
  EXPECT_EQ(cc->nbre_storage()->get(
                neb::configuration::instance().nbre_max_height_name()),
            neb::number_to_byte<neb::bytes>(height));

  auto ele = q.try_pop_front();
  EXPECT_TRUE(ele.first);
  EXPECT_EQ(height + 1, ele.second->get<p_height>());
  const auto &txs = ele.second->get<p_ir_transactions>();
  EXPECT_EQ(txs.size(), 1);
  auto tx_bytes = neb::string_to_byte(txs[0]);
  auto tx_ptr = std::make_unique<corepb::Transaction>();
  auto ret = tx_ptr->ParseFromArray(tx_bytes.value(), tx_bytes.size());
  EXPECT_TRUE(ret);
  EXPECT_EQ(tx_ptr->from(), "admin_pub_addr");
  EXPECT_EQ(tx_ptr->data().type(), "protocol");
}

TEST(test_ir_processor, gen_transactions_five) {
  neb::util::wakeable_queue<std::shared_ptr<nbre_ir_transactions_req>> q;
  std::vector<std::pair<protocol_type, int32_t>> v(
      {{protocol_auth, 1}, {protocol_nr, 2}, {protocol_dip, 2}});
  neb::block_height_t height = 1000;
  gen_transactions(height, v, q);
  EXPECT_EQ(cc->nbre_storage()->get(
                neb::configuration::instance().nbre_max_height_name()),
            neb::number_to_byte<neb::bytes>(height));

  for (size_t i = 0; i < v.size(); i++) {
    auto ele = q.try_pop_front();
    EXPECT_TRUE(ele.first);
    EXPECT_EQ(height + i + 1, ele.second->get<p_height>());
    const auto &txs = ele.second->get<p_ir_transactions>();
    EXPECT_EQ(txs.size(), 1);
    auto tx_bytes = neb::string_to_byte(txs[0]);
    auto tx_ptr = std::make_unique<corepb::Transaction>();
    auto ret = tx_ptr->ParseFromArray(tx_bytes.value(), tx_bytes.size());
    EXPECT_TRUE(ret);
    EXPECT_EQ(tx_ptr->data().type(), "protocol");

    if (i == 0) {
      EXPECT_EQ(tx_ptr->from(), "admin_pub_addr");
    } else if (i == 1 || i == 2) {
      EXPECT_EQ(tx_ptr->from(), "nr_addr");
    } else if (i == 3 || i == 4) {
      EXPECT_EQ(tx_ptr->from(), "dip_addr");
    }
  }
}

TEST(test_ir_processor, parse_irs_simple) {
  auto *ms_ptr = cc->nbre_storage();
  auto *bc_ptr = cc->blockchain();
  auto ip_ptr = std::make_unique<neb::fs::ir_processor>(ms_ptr, bc_ptr);

  neb::util::wakeable_queue<std::shared_ptr<nbre_ir_transactions_req>> q;
  std::vector<std::pair<protocol_type, int32_t>> v({{protocol_auth, 1}});
  neb::block_height_t height = 500;
  gen_transactions(height, v, q);

  ip_ptr->parse_irs(q);
  auto ir_with_version_ret = ip_ptr->get_ir_with_version("auth", 1);
  EXPECT_EQ(ir_with_version_ret->name(), "auth");
  EXPECT_EQ(ir_with_version_ret->version(), 1);
  EXPECT_EQ(ir_with_version_ret->height(), height + 1);
  EXPECT_EQ(ir_with_version_ret->ir_type(), neb::ir_type::llvm);
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_version("nr", 1));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_version("auth", 2));

  auto brief_ret = ip_ptr->get_ir_brief_key_with_height("auth", height + 1);
  EXPECT_EQ(*brief_ret, neb::concate_name_version("auth", 1));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("auth", height));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("nr", height));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("nr", height + 1));

  auto ir_with_height_ret = ip_ptr->get_ir_with_height("auth", height + 1);
  EXPECT_EQ(ir_with_height_ret->name(), "auth");
  EXPECT_EQ(ir_with_height_ret->version(), 1);
  EXPECT_EQ(ir_with_height_ret->height(), height + 1);
  EXPECT_EQ(ir_with_height_ret->ir_type(), neb::ir_type::llvm);
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_height("auth", height));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_height("nr", height + 1));

  auto ir_depends_ret = ip_ptr->get_ir_depends(*ir_with_height_ret);
  EXPECT_EQ(ir_depends_ret.size(), 1);
  EXPECT_EQ(ir_with_height_ret->name(), ir_depends_ret[0].name());
  EXPECT_EQ(ir_with_height_ret->version(), ir_depends_ret[0].version());
  EXPECT_EQ(ir_with_height_ret->height(), ir_depends_ret[0].height());
  EXPECT_EQ(ir_with_height_ret->ir_type(), ir_depends_ret[0].ir_type());

  auto ir_names = ip_ptr->get_ir_names();
  EXPECT_EQ(ir_names.size(), 1);
  EXPECT_EQ(ir_names[0], "auth");

  auto ir_versions = ip_ptr->get_ir_versions("auth");
  EXPECT_EQ(ir_versions.size(), 1);
  EXPECT_EQ(ir_versions[0], 1);
}

TEST(test_ir_processor, parse_irs_multi) {
  auto *ms_ptr = cc->nbre_storage();
  auto *bc_ptr = cc->blockchain();
  auto ip_ptr = std::make_unique<neb::fs::ir_processor>(ms_ptr, bc_ptr);

  neb::util::wakeable_queue<std::shared_ptr<nbre_ir_transactions_req>> q;
  std::vector<std::pair<protocol_type, int32_t>> v(
      {{protocol_auth, 1}, {protocol_nr, 2}, {protocol_dip, 2}});
  neb::block_height_t height = 1000;
  gen_transactions(height, v, q);

  ip_ptr->parse_irs(q);
  auto ir_with_version_ret = ip_ptr->get_ir_with_version("auth", 1);
  auto ir_cmp = [](const nbre::NBREIR &ir, const std::string &name,
                   neb::version_t version, neb::block_height_t height) {
    EXPECT_EQ(ir.name(), name);
    EXPECT_EQ(ir.version(), version);
    EXPECT_EQ(ir.height(), height);
    EXPECT_EQ(ir.ir_type(), neb::ir_type::llvm);
  };
  ir_cmp(*ir_with_version_ret, "auth", 1, height + 1);
  ir_with_version_ret = ip_ptr->get_ir_with_version("nr", 1);
  ir_cmp(*ir_with_version_ret, "nr", 1, height + 2);
  ir_with_version_ret = ip_ptr->get_ir_with_version("nr", 2);
  ir_cmp(*ir_with_version_ret, "nr", 2, height + 3);
  ir_with_version_ret = ip_ptr->get_ir_with_version("dip", 1);
  ir_cmp(*ir_with_version_ret, "dip", 1, height + 4);
  ir_with_version_ret = ip_ptr->get_ir_with_version("dip", 2);
  ir_cmp(*ir_with_version_ret, "dip", 2, height + 5);
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_version("auth", 2));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_version("nr", 3));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_version("dip", 3));

  auto brief_ret = ip_ptr->get_ir_brief_key_with_height("auth", height + 1);
  EXPECT_EQ(*brief_ret, neb::concate_name_version("auth", 1));
  brief_ret = ip_ptr->get_ir_brief_key_with_height("nr", height + 2);
  EXPECT_EQ(*brief_ret, neb::concate_name_version("nr", 1));
  brief_ret = ip_ptr->get_ir_brief_key_with_height("nr", height + 3);
  EXPECT_EQ(*brief_ret, neb::concate_name_version("nr", 2));
  brief_ret = ip_ptr->get_ir_brief_key_with_height("dip", height + 4);
  EXPECT_EQ(*brief_ret, neb::concate_name_version("dip", 1));
  brief_ret = ip_ptr->get_ir_brief_key_with_height("dip", height + 5);
  EXPECT_EQ(*brief_ret, neb::concate_name_version("dip", 2));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("auth", height));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("nr", height + 1));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_brief_key_with_height("dip", height + 3));

  auto obj_ir_cmp = [](const nbre::NBREIR &sir, const nbre::NBREIR &tir) {
    EXPECT_EQ(sir.name(), tir.name());
    EXPECT_EQ(sir.version(), tir.version());
    EXPECT_EQ(sir.height(), tir.height());
    EXPECT_EQ(sir.ir_type(), tir.ir_type());
  };
  auto ir_with_height_ret = ip_ptr->get_ir_with_height("auth", height + 1);
  ir_cmp(*ir_with_height_ret, "auth", 1, height + 1);
  auto ir_depends_ret = ip_ptr->get_ir_depends(*ir_with_height_ret);
  EXPECT_EQ(ir_depends_ret.size(), 1);
  obj_ir_cmp(*ir_with_height_ret, ir_depends_ret[0]);
  ir_with_height_ret = ip_ptr->get_ir_with_height("nr", height + 2);
  ir_cmp(*ir_with_height_ret, "nr", 1, height + 2);
  ir_depends_ret = ip_ptr->get_ir_depends(*ir_with_height_ret);
  EXPECT_EQ(ir_depends_ret.size(), 1);
  obj_ir_cmp(*ir_with_height_ret, ir_depends_ret[0]);
  ir_with_height_ret = ip_ptr->get_ir_with_height("nr", height + 3);
  ir_cmp(*ir_with_height_ret, "nr", 2, height + 3);
  ir_depends_ret = ip_ptr->get_ir_depends(*ir_with_height_ret);
  EXPECT_EQ(ir_depends_ret.size(), 1);
  obj_ir_cmp(*ir_with_height_ret, ir_depends_ret[0]);
  ir_with_height_ret = ip_ptr->get_ir_with_height("dip", height + 4);
  ir_cmp(*ir_with_height_ret, "dip", 1, height + 4);
  ir_depends_ret = ip_ptr->get_ir_depends(*ir_with_height_ret);
  EXPECT_EQ(ir_depends_ret.size(), 2);
  obj_ir_cmp(*ir_with_height_ret, ir_depends_ret[0]);
  ir_cmp(ir_depends_ret[1], "nr", 1, height + 2);
  ir_with_height_ret = ip_ptr->get_ir_with_height("dip", height + 5);
  ir_cmp(*ir_with_height_ret, "dip", 2, height + 5);
  ir_depends_ret = ip_ptr->get_ir_depends(*ir_with_height_ret);
  EXPECT_EQ(ir_depends_ret.size(), 2);
  obj_ir_cmp(*ir_with_height_ret, ir_depends_ret[0]);
  ir_cmp(ir_depends_ret[1], "nr", 2, height + 3);
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_height("auth", height));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_height("nr", height + 1));
  EXPECT_EQ(neb::none, ip_ptr->get_ir_with_height("dip", height + 3));

  auto ir_names = ip_ptr->get_ir_names();
  EXPECT_EQ(ir_names.size(), 3);
  EXPECT_TRUE(std::find(ir_names.begin(), ir_names.end(), "auth") !=
              ir_names.end());
  EXPECT_TRUE(std::find(ir_names.begin(), ir_names.end(), "nr") !=
              ir_names.end());
  EXPECT_TRUE(std::find(ir_names.begin(), ir_names.end(), "dip") !=
              ir_names.end());

  auto ir_versions = ip_ptr->get_ir_versions("auth");
  EXPECT_EQ(ir_versions.size(), 1);
  EXPECT_EQ(ir_versions[0], 1);
  ir_versions = ip_ptr->get_ir_versions("nr");
  EXPECT_EQ(ir_versions.size(), 2);
  EXPECT_EQ(ir_versions[0], 1);
  EXPECT_EQ(ir_versions[1], 2);
  ir_versions = ip_ptr->get_ir_versions("dip");
  EXPECT_EQ(ir_versions.size(), 2);
  EXPECT_EQ(ir_versions[0], 1);
  EXPECT_EQ(ir_versions[1], 2);
}
