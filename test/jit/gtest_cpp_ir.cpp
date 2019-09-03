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

#include "common/byte.h"
#include "fs/proto/ir.pb.h"
#include "jit/cpp_ir.h"
#include "test/jit/ir/gen_ir.h"
#include <gtest/gtest.h>

TEST(test_cpp_ir, llvm_ir_content) {
  auto ir = gen_auth_ir();
  std::stringstream ss;
  ss << ir.name();
  ss << ir.version();
  neb::cpp::cpp_ir ci(std::make_pair(ss.str(), ir.ir()));
  neb::bytes ir_content = ci.llvm_ir_content();

}
