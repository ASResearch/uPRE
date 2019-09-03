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

#include "common/configuration.h"
#include "jit/jit_mangled_entry_point.h"
#include "test/gtest_common.h"
#include <gtest/gtest.h>

TEST(test_jit_mangled_entry_point, get_mangled_entry_name) {
  auto ret = std::make_unique<neb::jit::jit_mangled_entry_point>();

  auto auth_func_name = neb::configuration::instance().auth_func_name();
  auto auth_func_mangled = ret->get_mangled_entry_name(auth_func_name);
  EXPECT_EQ(auth_func_mangled, "_Z16entry_point_authB5cxx11v");

  auto nr_func_name = neb::configuration::instance().nr_func_name();
  auto nr_func_mangled = ret->get_mangled_entry_name(nr_func_name);
  EXPECT_EQ(nr_func_mangled, "_Z14entry_point_nryy");

  auto dip_func_name = neb::configuration::instance().dip_func_name();
  auto dip_func_mangled = ret->get_mangled_entry_name(dip_func_name);
  EXPECT_EQ(dip_func_mangled, "_Z15entry_point_dipy");

  auto nr_param_func_name = neb::configuration::instance().nr_param_func_name();
  auto nr_param_func_mangled = ret->get_mangled_entry_name(nr_param_func_name);
  EXPECT_EQ(nr_param_func_mangled, "_Z12get_nr_paramv");

  auto dip_param_func_name =
      neb::configuration::instance().dip_param_func_name();
  auto dip_param_func_mangled =
      ret->get_mangled_entry_name(dip_param_func_name);
  EXPECT_EQ(dip_param_func_mangled, "_Z13get_dip_paramv");

  auth_func_mangled = ret->get_mangled_entry_name(auth_func_name);
  EXPECT_EQ(auth_func_mangled, "_Z16entry_point_authB5cxx11v");

  nr_func_mangled = ret->get_mangled_entry_name(nr_func_name);
  EXPECT_EQ(nr_func_mangled, "_Z14entry_point_nryy");

  dip_func_mangled = ret->get_mangled_entry_name(dip_func_name);
  EXPECT_EQ(dip_func_mangled, "_Z15entry_point_dipy");
}

TEST(test_jit_mangled_entry_point, get_mangled_entry_name_illegal) {
  auto jm = std::make_unique<neb::jit::jit_mangled_entry_point>();

  auto ret = jm->get_mangled_entry_name(std::string());
  EXPECT_EQ(ret, std::string());

  ret = jm->get_mangled_entry_name("_Z16entry_point_authB5cxx11v");
  EXPECT_EQ(ret, std::string());
}

