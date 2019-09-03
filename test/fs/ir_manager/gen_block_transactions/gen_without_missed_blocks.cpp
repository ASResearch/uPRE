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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the go-nebulas library.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "test/fs/ir_manager/gen_block_transactions/gen_without_missed_blocks.h"
#include "common/configuration.h"
#include "fs/proto/block.pb.h"
#include "fs/proto/ir.pb.h"
#include "fs/storage.h"
#include "test/fs/ir_manager/gen_ir/gen_auth_ir.h"
#include "test/fs/ir_manager/gen_ir/gen_dip_ir.h"
#include "test/fs/ir_manager/gen_ir/gen_nr_ir.h"
#include "test/gtest_common.h"
#include "util/json_parser.h"
#include <boost/property_tree/json_parser.hpp>

std::string gen_transaction_payload(const protocol_type &type,
                                    const neb::block_height_t &height) {
  nbre::NBREIR ir;
  switch (type) {
  case protocol_auth:
    static neb::version_t auth_version = 1;
    ir = gen_auth_ir_raw("auth", auth_version++, height);
    break;
  case protocol_nr:
    static neb::version_t nr_version = 1;
    ir = gen_nr_ir_raw("nr", nr_version++, height);
    break;
  case protocol_dip:
    static neb::version_t dip_version = 1;
    ir = gen_dip_ir_raw("dip", dip_version++, height);
    break;
  default:
    break;
  }

  auto val_str = ir.SerializeAsString();

  boost::property_tree::ptree pt;
  pt.put("Data", neb::string_to_byte(val_str).to_base64());
  std::string ret;
  neb::util::json_parser::write_json(ret, pt);
  return ret;
}

std::string gen_from_address(const protocol_type &type) {
  switch (type) {
  case protocol_auth:
    return neb::byte_to_string(neb::configuration::instance().admin_pub_addr());
    break;
  case protocol_nr:
    return "nr_addr";
    break;
  case protocol_dip:
    return "dip_addr";
    break;
  default:
    break;
  }
}

std::string gen_transaction(const protocol_type &type,
                            const neb::block_height_t &height) {
  corepb::Transaction tx;
  auto *data = tx.mutable_data();
  data->set_type("protocol");
  data->set_payload(gen_transaction_payload(type, height));
  tx.set_from(gen_from_address(type));
  return tx.SerializeAsString();
}

void gen_transactions(
    const neb::block_height_t &start_height,
    const std::vector<std::pair<protocol_type, int32_t>> &type_and_cnt,
    neb::util::wakeable_queue<std::shared_ptr<nbre_ir_transactions_req>> &q) {

  auto height = start_height;
  cc->nbre_storage()->put(neb::configuration::instance().nbre_max_height_name(),
                          neb::number_to_byte<neb::bytes>(height));

  for (auto &ele : type_and_cnt) {
    int32_t tmp_cnt = ele.second;
    while (tmp_cnt--) {
      auto req_ptr = std::make_shared<nbre_ir_transactions_req>();
      req_ptr->set<p_height>(++height);
      std::vector<std::string> txs;
      txs.push_back(gen_transaction(ele.first, height));
      req_ptr->set<p_ir_transactions>(txs);
      q.push_back(req_ptr);
    }
  }
}
