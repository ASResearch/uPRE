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

#include "test/fs/ir_manager/gen_ir/gen_dip_ir.h"
#include "common/common.h"
#include "common/version.h"
#include "fs/proto/ir.pb.h"
#include <boost/algorithm/string/replace.hpp>

static std::string dip_slice =
    "#include \"runtime/dip/dip_impl.h\" \n"
    "\n"
    "extern neb::rt::nr::nr_ret_type \n"
    "entry_point_nr(neb::compatible_uint64_t start_block, \n"
    "               neb::compatible_uint64_t end_block); \n"
    "\n"
    "auto to_version_t = [](uint32_t major_version, uint16_t minor_version, \n"
    "                       uint16_t patch_version) -> neb::version_t { \n"
    "  return (0ULL + major_version) + ((0ULL + minor_version) << 32) + \n"
    "         ((0ULL + patch_version) << 48); \n"
    "}; \n"
    "\n"
    "neb::rt::dip::dip_param_t get_dip_param() { \n"
    "  neb::rt::dip::dip_param_t ret; \n"
    "  ret.set<p_start_block>(VAR_height); \n"
    "  ret.set<p_version>(to_version_t(VERSION_major, VERSION_minor, "
    "VERSION_patch)); \n"
    "  return ret; \n"
    "} \n"
    "neb::rt::dip::dip_ret_type entry_point_dip(neb::compatible_uint64_t "
    "height) { \n"
    "  neb::rt::dip::dip_ret_type ret; \n"
    "\n"
    "  uint64_t block_nums_of_a_day = VAR_block_nums_of_a_day; \n"
    "  uint64_t days = VAR_days; \n"
    "  neb::compatible_uint64_t dip_start_block = VAR_dip_start_block; \n"
    "  neb::compatible_uint64_t block_interval = days * block_nums_of_a_day; \n"
    "  std::string reward_addr = \"VAR_reward_addr\"; \n"
    "  std::string coinbase_addr = \"VAR_coinbase_addr\"; \n"
    "\n"
    "  uint64_t interval_nums = (height - dip_start_block) / block_interval; \n"
    "  neb::compatible_uint64_t s = dip_start_block + block_interval * "
    "interval_nums; \n"
    "  neb::compatible_uint64_t e = s - 1; \n"
    "  s -= block_interval; \n"
    "\n"
    "  auto nr_ret = entry_point_nr(s, e); \n"
    "\n"
    "  neb::rt::dip::dip_float_t alpha = VAR_alpha; \n"
    "  neb::rt::dip::dip_float_t beta = VAR_beta; \n"
    "  return neb::rt::dip::entry_point_dip_impl(s, e, "
    "to_version_t(VERSION_major, VERSION_minor, VERSION_patch), height, "
    "nr_ret, VAR_alpha, VAR_beta); \n"
    "} \n";

std::string gen_dip_with_params(uint64_t block_nums_of_a_day, uint64_t days,
                                uint64_t dip_start_block,
                                const std::string &reward_addr,
                                const std::string &coinbase_addr, float alpha,
                                float beta, int32_t major_version,
                                int32_t minor_version, int32_t patch_version) {
  std::string ret = dip_slice;
  std::stringstream ss;
#define R(t, name)                                                             \
  ss << t;                                                                     \
  boost::replace_all(ret, name, std::to_string(t));                            \
  ss.clear();

  R(block_nums_of_a_day, "VAR_block_nums_of_a_day");
  R(days, "VAR_days");
  R(dip_start_block, "VAR_dip_start_block");
  R(alpha, "VAR_alpha");
  R(beta, "VAR_beta");
  R(major_version, "VERSION_major");
  R(minor_version, "VERSION_minor");
  R(patch_version, "VERSION_patch");
  R(dip_start_block, "VAR_height");
#undef R

#define R_STR(t, name)                                                         \
  ss << t;                                                                     \
  boost::replace_all(ret, name, t);                                            \
  ss.clear();
  R_STR(reward_addr, "VAR_reward_addr");
  R_STR(coinbase_addr, "VAR_coinbase_addr");
#undef R_STR

  return ret;
}

nbre::NBREIR gen_dip_ir_raw(const std::string &name, neb::version_t version,
                            neb::block_height_t height) {
  neb::version v(version);
  std::string payload = gen_dip_with_params(
      10, 2, height, "reward_addr", "coinbase_addr", 1, 1, v.major_version(),
      v.minor_version(), v.patch_version());
  nbre::NBREIR ir;
  ir.set_name(name);
  ir.set_version(version);
  ir.set_height(height);
  ir.set_ir(payload);
  ir.set_ir_type(neb::ir_type::cpp);

  auto deps_ptr = ir.add_depends();
  deps_ptr->set_name("nr");
  deps_ptr->set_version(version);

  return ir;
}
