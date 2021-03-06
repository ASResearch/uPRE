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

#pragma once
#include "common/address.h"
#include "fs/blockchain/data_type.h"
#include "runtime/dip/data_type.h"

namespace neb {
namespace rt {
namespace dip {

class dip_cache;
class dip_calculator;

class dip_reward {
public:
  dip_reward(dip_calculator *calculator, dip_cache *cache);

  virtual std::vector<dip_item> get_dip_reward(
      neb::block_height_t start_block, neb::block_height_t end_block,
      neb::block_height_t height, const std::vector<nr_item> &nr_result,
      neb::fs::transaction_db_interface *tdb_ptr,
      neb::fs::account_db_interface *adb_ptr, floatxx_t alpha, floatxx_t beta);

protected:
  dip_calculator *m_calculator;
  dip_cache *m_cache;
};

} // namespace dip
} // namespace rt
} // namespace neb
