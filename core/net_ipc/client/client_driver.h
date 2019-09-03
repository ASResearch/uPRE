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
#pragma once
#include "core/net_ipc/client/client_driver_base.h"
#include "core/nr_handler.h"

namespace neb {
namespace core {

class client_driver : public internal::client_driver_base {
public:
  client_driver();
  virtual ~client_driver();

protected:
  virtual void add_handlers();

  void add_nr_handlers();

  void add_dip_handlers();

protected:
  std::unique_ptr<nr_handler> m_nr_handler;
};
} // namespace core
} // namespace neb
