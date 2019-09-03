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

#pragma once

#include "core/net_ipc/nipc_pkg.h"
#include "util/wakeable_queue.h"

enum protocol_type {
  protocol_auth = 0,
  protocol_nr,
  protocol_dip,
};

void gen_transactions(
    const neb::block_height_t &start_height,
    const std::vector<std::pair<protocol_type, int32_t>> &type_and_cnt,
    neb::util::wakeable_queue<std::shared_ptr<nbre_ir_transactions_req>> &q);

