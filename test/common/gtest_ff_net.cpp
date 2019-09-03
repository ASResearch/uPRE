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

#include "runtime/dip/data_type.h"
#include "runtime/nr/impl/data_type.h"
#include <ff/network.h>
#include <gtest/gtest.h>

define_nt(p_i, int32_t);
typedef ::ff::net::ntpackage<10, p_i> pi_t;
TEST(test_ff_net, int) {
  pi_t pi;
  pi.get<p_i>() = int();
  auto pi_str = pi.serialize_to_string();
  pi_t ret;
  ret.deserialize_from_string(pi_str);
  EXPECT_EQ(int(), ret.get<p_i>());

  pi.get<p_i>() = 123;
  pi_str = pi.serialize_to_string();
  ret.deserialize_from_string(pi_str);
  EXPECT_EQ(123, ret.get<p_i>());
}

define_nt(p_s, std::string);
typedef ::ff::net::ntpackage<10, p_s> ps_t;
TEST(test_ff_net, string) {
  ps_t ps;
  ps.get<p_s>() = std::string();
  auto ps_str = ps.serialize_to_string();
  ps_t ret;
  ret.deserialize_from_string(ps_str);
  EXPECT_EQ(std::string(), ret.get<p_s>());

  ps.get<p_s>() = std::string("hah");
  ps_str = ps.serialize_to_string();
  ret.deserialize_from_string(ps_str);
  EXPECT_EQ(std::string("hah"), ret.get<p_s>());
}

define_nt(p_b, neb::bytes);
typedef ::ff::net::ntpackage<10, p_b> pb_t;
TEST(test_ff_net, bytes) {
  pb_t pb;
  pb.get<p_b>() = neb::bytes();
  auto pb_str = pb.serialize_to_string();
  pb_t ret;
  ret.deserialize_from_string(pb_str);
  EXPECT_EQ(neb::bytes(), ret.get<p_b>());

  pb.get<p_b>() = neb::string_to_byte("xix");
  pb_str = pb.serialize_to_string();
  ret.deserialize_from_string(pb_str);
  EXPECT_EQ(neb::string_to_byte("xix"), ret.get<p_b>());
}

define_nt(p_f, neb::floatxx_t);
typedef ::ff::net::ntpackage<10, p_f> pf_t;
TEST(test_ff_net, floatxx_t) {
  pf_t pf;
  pf.get<p_f>() = neb::floatxx_t();
  auto pf_str = pf.serialize_to_string();
  pf_t ret;
  ret.deserialize_from_string(pf_str);
  EXPECT_EQ(neb::floatxx_t(), ret.get<p_f>());

  neb::floatxx_t ten =
      softfloat_cast<uint32_t, typename neb::floatxx_t::value_type>(10);
  pf.get<p_f>() = ten;
  pf_str = pf.serialize_to_string();
  ret.deserialize_from_string(pf_str);
  EXPECT_EQ(ten, ret.get<p_f>());

  neb::floatxx_t one =
      softfloat_cast<uint32_t, typename neb::floatxx_t::value_type>(1);
  neb::floatxx_t two =
      softfloat_cast<uint32_t, typename neb::floatxx_t::value_type>(2);
  neb::floatxx_t one_and_half = one / two + one;
  pf.get<p_f>() = one_and_half;
  pf_str = pf.serialize_to_string();
  ret.deserialize_from_string(pf_str);
  EXPECT_EQ(one_and_half, ret.get<p_f>());
}

define_nt(p_vs, std::vector<std::string>);
typedef ::ff::net::ntpackage<10, p_vs> pvs_t;
TEST(test_ff_net, v_string) {
  std::string hah = std::string("hah");
  std::string xix = std::string("xix");
  std::string heh = std::string("heh");

  pvs_t pvs;
  auto &tmp = pvs.get<p_vs>();
  tmp.push_back(hah);
  tmp.push_back(xix);
  auto pvs_str = pvs.serialize_to_string();

  pvs_t ret;
  ret.deserialize_from_string(pvs_str);
  auto &temp = ret.get<p_vs>();
  EXPECT_EQ(tmp.size(), 2);
  EXPECT_EQ(tmp[0], hah);
  EXPECT_EQ(tmp[1], xix);

  temp.push_back(heh);
  pvs_str = ret.serialize_to_string();
  pvs.deserialize_from_string(pvs_str);
  tmp = pvs.get<p_vs>();
  EXPECT_EQ(tmp.size(), 3);
  EXPECT_EQ(tmp[0], hah);
  EXPECT_EQ(tmp[1], xix);
  EXPECT_EQ(tmp[2], heh);
}

define_nt(p_vb, std::vector<neb::bytes>);
typedef ::ff::net::ntpackage<10, p_vb> pvb_t;
TEST(test_ff_net, v_bytes) {
  neb::bytes hah = neb::string_to_byte("hah");
  neb::bytes xix = neb::string_to_byte("xix");
  neb::bytes heh = neb::string_to_byte("heh");

  pvb_t pvb;
  auto &tmp = pvb.get<p_vb>();
  tmp.push_back(hah);
  tmp.push_back(xix);
  auto pvb_str = pvb.serialize_to_string();

  pvb_t ret;
  ret.deserialize_from_string(pvb_str);
  auto &temp = ret.get<p_vb>();
  EXPECT_EQ(tmp.size(), 2);
  EXPECT_EQ(tmp[0], hah);
  EXPECT_EQ(tmp[1], xix);

  temp.push_back(heh);
  pvb_str = ret.serialize_to_string();
  pvb.deserialize_from_string(pvb_str);
  tmp = pvb.get<p_vb>();
  EXPECT_EQ(tmp.size(), 3);
  EXPECT_EQ(tmp[0], hah);
  EXPECT_EQ(tmp[1], xix);
  EXPECT_EQ(tmp[2], heh);
}

TEST(test_ff_net, nr_param_t) {
  neb::rt::nr::nr_param_t np;
  np.set<p_start_block>(100);
  np.set<p_block_interval>(20);
  np.set<p_version>(1);
  auto np_str = np.serialize_to_string();

  neb::rt::nr::nr_param_t ret;
  ret.deserialize_from_string(np_str);
  EXPECT_EQ(ret.get<p_start_block>(), 100);
  EXPECT_EQ(ret.get<p_block_interval>(), 20);
  EXPECT_EQ(ret.get<p_version>(), 1);
}

TEST(test_ff_net, dip_param_t) {
  neb::rt::dip::dip_param_t dp;
  dp.set<p_start_block>(100);
  dp.set<p_block_interval>(20);
  dp.set<p_version>(1);
  dp.set<p_dip_reward_addr>("123");
  dp.set<p_dip_coinbase_addr>("456");
  auto dp_str = dp.serialize_to_string();

  neb::rt::dip::dip_param_t ret;
  ret.deserialize_from_string(dp_str);
  EXPECT_EQ(ret.get<p_start_block>(), 100);
  EXPECT_EQ(ret.get<p_block_interval>(), 20);
  EXPECT_EQ(ret.get<p_version>(), 1);
  EXPECT_EQ(ret.get<p_dip_reward_addr>(), "123");
  EXPECT_EQ(ret.get<p_dip_coinbase_addr>(), "456");
}

define_nt(p_vps, std::vector<ps_t>);
typedef ::ff::net::ntpackage<10, p_vps> pvps_t;
TEST(test_ff_net, v_ps_v) {
  pvps_t pvps;
  auto &tmp = pvps.get<p_vps>();

  ps_t ps_0;
  ps_0.get<p_s>() = "hah";
  tmp.push_back(ps_0);

  ps_t ps_1;
  ps_1.set<p_s>("xix");
  tmp.push_back(ps_1);

  auto pvps_str = pvps.serialize_to_string();
  pvps_t ret;
  ret.deserialize_from_string(pvps_str);

  EXPECT_EQ(tmp.size(), 2);
  EXPECT_EQ("hah", ret.get<p_vps>()[0].get<p_s>());
  EXPECT_EQ("xix", ret.get<p_vps>()[1].get<p_s>());
}

