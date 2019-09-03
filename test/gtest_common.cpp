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

#include "test/gtest_common.h"
#include "common/configuration.h"
#include "compatible/compatible_checker.h"
#include "compatible/db_checker.h"
#include "fs/blockchain.h"
#include "fs/ir_manager/api/ir_list.h"
#include "fs/ir_manager/ir_processor.h"
#include "fs/memory_storage.h"
#include "fs/rocksdb_session_storage.h"
#include "fs/rocksdb_storage.h"
#include "runtime/auth/auth_handler.h"
#include "runtime/auth/auth_table.h"
#include "util/persistent_flag.h"
#include "util/persistent_type.h"

neb::core::client_context *init_context() {
  auto *cc_ptr = (neb::core::client_context *)neb::core::context.get();

  cc_ptr->m_bc_storage = std::make_unique<neb::fs::memory_storage>();
  auto *rss = (neb::fs::memory_storage *)cc_ptr->blockchain_storage();
  // rss->init(neb::configuration::instance().neb_db_dir(),
  // neb::fs::storage_open_for_readonly);

  cc_ptr->m_nbre_storage = std::make_unique<neb::fs::memory_storage>();
  auto *rs = (neb::fs::memory_storage *)cc_ptr->nbre_storage();
  // rs->open_database(neb::configuration::instance().nbre_db_dir(),
  // neb::fs::storage_open_for_readwrite);

  cc_ptr->m_blockchain =
      std::make_unique<neb::fs::blockchain>(cc_ptr->blockchain_storage());

  cc_ptr->m_ir_processor = std::make_unique<neb::fs::ir_processor>(
      cc_ptr->nbre_storage(), cc_ptr->blockchain());

  neb::configuration::instance().admin_pub_addr() =
      neb::string_to_byte("admin_pub_addr");

  cc_ptr->m_compatible_checker =
      std::make_unique<neb::compatible::compatible_checker>();
  cc_ptr->m_compatible_checker->init();

  return cc_ptr;
}

void release_context(neb::core::client_context *cc_ptr) {
  cc_ptr->shutdown();
  neb::core::context.reset();
}

neb::core::client_context *cc = init_context();
