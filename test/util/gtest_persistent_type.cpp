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
#include "util/persistent_type.h"
#include <gtest/gtest.h>

TEST(test_persistent_type, simple) {
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto pt_ptr =
      std::make_unique<neb::util::persistent_type<neb::block_height_t>>(
          ms_ptr.get(), "hah");

  EXPECT_EQ(0, pt_ptr->get());
  pt_ptr->set(100);
  EXPECT_EQ(100, pt_ptr->get());
  pt_ptr->clear();
  EXPECT_EQ(0, pt_ptr->get());
}
