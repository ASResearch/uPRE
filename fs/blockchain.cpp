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

#include "fs/blockchain.h"
#include "common/byte.h"
#include "fs/storage.h"

namespace neb {
namespace fs {

blockchain::blockchain(class storage *db) : m_storage(db) {}
blockchain::~blockchain() {}

std::unique_ptr<corepb::Block> blockchain::load_LIB_block() {
  return load_block_with_tag_string(Block_LIB);
}

std::unique_ptr<corepb::Block>
blockchain::load_block_with_height(block_height_t height) {
  std::unique_ptr<corepb::Block> block = std::make_unique<corepb::Block>();

  neb::bytes height_hash =
      m_storage->get_bytes(neb::number_to_byte<neb::bytes>(height));

  neb::bytes block_bytes = m_storage->get_bytes(height_hash);

  bool ret = block->ParseFromArray(block_bytes.value(), block_bytes.size());
  if (!ret) {
    throw std::runtime_error("parse block failed");
  }
  return block;
}

std::unique_ptr<corepb::Block>
blockchain::load_block_with_tag_string(const std::string &tag) {

  std::unique_ptr<corepb::Block> block = std::make_unique<corepb::Block>();
  neb::bytes tail_hash = m_storage->get_bytes(neb::string_to_byte(tag));

  neb::bytes block_bytes = m_storage->get_bytes(tail_hash);

  bool ret = block->ParseFromArray(block_bytes.value(), block_bytes.size());
  if (!ret) {
    throw std::runtime_error("parse block failed");
  }
  return block;
}

void blockchain::write_LIB_block(corepb::Block *block) {
  if (block == nullptr)
    return;

  block_height_t height = block->height();
  auto height_hash = block->header().hash();
  m_storage->put_bytes(number_to_byte<neb::bytes>(height),
                       string_to_byte(height_hash));

  m_storage->put_bytes(string_to_byte(height_hash),
                       string_to_byte(block->SerializeAsString()));

  m_storage->put_bytes(string_to_byte(Block_LIB), string_to_byte(height_hash));
}
storage *blockchain::storage() { return m_storage; }
} // namespace fs
} // namespace neb

