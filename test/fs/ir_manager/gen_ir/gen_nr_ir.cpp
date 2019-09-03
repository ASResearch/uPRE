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

#include "test/fs/ir_manager/gen_ir/gen_nr_ir.h"
#include "common/common.h"
#include "common/version.h"
#include "fs/proto/ir.pb.h"
#include <boost/algorithm/string/replace.hpp>

static std::string ir_slice =
    "#include \"runtime/nr/impl/nr_impl.h\" \n"
    "auto to_version_t = [](uint32_t major_version, uint16_t minor_version, "
    "                       uint16_t patch_version) -> "
    "neb::version_t { \n"
    "  return (0ULL + major_version) + ((0ULL + minor_version) << 32) + \n"
    "         ((0ULL + patch_version) << 48); \n"
    "}; \n"
    "neb::rt::nr::nr_param_t get_nr_param() { \n"
    "  neb::rt::nr::nr_param_t ret; \n"
    "  ret.set<p_start_block>(VAR_height); \n"
    "  ret.set<p_version>(to_version_t(VERSION_major, VERSION_minor, "
    "VERSION_patch)); \n"
    "  return ret; \n"
    "} \n"
    "neb::rt::nr::nr_ret_type entry_point_nr(neb::compatible_uint64_t "
    "start_block, \n"
    "                           neb::compatible_uint64_t end_block) { \n"
    "  neb::rt::nr::nr_ret_type ret; \n"
    "  uint64_t block_nums_of_a_day = 24 * 3600 / 15; \n"
    "  uint64_t days = 7; \n"
    "  uint64_t block_interval = days * block_nums_of_a_day; \n"
    "  neb::compatible_int64_t a = VAR_a; \n"
    "  neb::compatible_int64_t b = VAR_b; \n"
    "  neb::compatible_int64_t c = VAR_c; \n"
    "  neb::compatible_int64_t d = VAR_d; \n"
    "  neb::rt::nr::nr_float_t theta = VAR_theta; \n"
    "  neb::rt::nr::nr_float_t mu = VAR_mu; \n"
    "  neb::rt::nr::nr_float_t lambda = VAR_lambda; \n"
    "  return neb::rt::nr::entry_point_nr_impl(start_block, end_block, \n"
    "                                          to_version_t(VERSION_major, "
    "VERSION_minor, VERSION_patch), a, b, c, "
    "d, \n"
    "                                          theta, mu, lambda); \n"
    "} \n";

std::string gen_ir_with_params(int64_t a, int64_t b, int64_t c, int64_t d,
                               float theta, float mu, float lambda,
                               int32_t major_version, int32_t minor_version,
                               int32_t patch_version,
                               neb::block_height_t height) {
  std::string ret = ir_slice;
  std::stringstream ss;
#define R(t, name)                                                             \
  ss << t;                                                                     \
  boost::replace_all(ret, name, std::to_string(t));                            \
  ss.clear();

  R(a, "VAR_a");
  R(b, "VAR_b");
  R(c, "VAR_c");
  R(d, "VAR_d");
  R(theta, "VAR_theta");
  R(mu, "VAR_mu");
  R(lambda, "VAR_lambda");
  R(major_version, "VERSION_major");
  R(minor_version, "VERSION_minor");
  R(patch_version, "VERSION_patch");
  R(height, "VAR_height");
#undef R

  return ret;
}

nbre::NBREIR gen_nr_ir_raw(const std::string &name, neb::version_t version,
                           neb::block_height_t height) {
  neb::version v(version);
  std::string payload =
      gen_ir_with_params(1, 2, 3, 4, 1, 1, 1, v.major_version(),
                         v.minor_version(), v.patch_version(), height);
  nbre::NBREIR ir;
  ir.set_name(name);
  ir.set_version(version);
  ir.set_height(height);
  ir.set_ir(payload);
  ir.set_ir_type(neb::ir_type::cpp);
  return ir;
}
