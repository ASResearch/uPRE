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

#include "runtime/nr/impl/nr_impl.h"

neb::rt::nr::nr_ret_type entry_point_nr(neb::compatible_uint64_t start_block,
                                        neb::compatible_uint64_t end_block) {
  auto to_version_t = [](uint32_t major_version, uint16_t minor_version,
                         uint16_t patch_version) -> neb::version_t {
    return (0ULL + major_version) + ((0ULL + minor_version) << 32) +
           ((0ULL + patch_version) << 48);
  };

  neb::compatible_uint64_t a = 100;
  neb::compatible_uint64_t b = 2;
  neb::compatible_uint64_t c = 6;
  neb::compatible_uint64_t d = -9;
  neb::rt::nr::nr_float_t theta = 1;
  neb::rt::nr::nr_float_t mu = 1;
  neb::rt::nr::nr_float_t lambda = 2;
  return neb::rt::nr::entry_point_nr_impl(start_block, end_block,
                                          to_version_t(1, 0, 0), a, b, c, d,
                                          theta, mu, lambda);
}

