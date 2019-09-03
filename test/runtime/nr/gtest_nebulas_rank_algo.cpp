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

#include "common/common.h"
#include "common/math/internal/math_extension.h"
#include "fs/blockchain.h"
#include "fs/blockchain/account/account_db.h"
#include "fs/blockchain/blockchain_api.h"
#include "fs/memory_storage.h"
#include "runtime/nr/impl/nebulas_rank_algo.h"
#include "runtime/util.h"
#include <gtest/gtest.h>
#include <random>
#define PRECESION 1e-5

template <typename T> T precesion(const T &x, float pre = PRECESION) {
  return std::fabs(T(x * pre));
}

TEST(test_runtime_nebulas_rank, split_transactions_by_block_interval) {
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  std::vector<neb::fs::transaction_info_t> txs;
  auto ret = algo_ptr->split_transactions_by_block_interval(txs);
  EXPECT_TRUE(ret.empty());

  neb::fs::transaction_info_t tx;
  txs.push_back(tx);
  ret = algo_ptr->split_transactions_by_block_interval(txs, 0);
  EXPECT_TRUE(ret.empty());

  ret = algo_ptr->split_transactions_by_block_interval(txs, 1);
  EXPECT_TRUE(!ret.empty());

  txs.clear();
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, std::numeric_limits<int16_t>::max());

  //  block interval default 128
  int32_t txs_size = dis(mt);
  for (int32_t i = 0; i < txs_size; i++) {
    tx.m_height = i;
    txs.push_back(tx);
  }
  ret = algo_ptr->split_transactions_by_block_interval(txs);
  size_t actual_txs_size = 0;
  for (auto &ele : ret) {
    actual_txs_size += ele.size();
  }
  EXPECT_EQ(txs_size, actual_txs_size);
  for (size_t i = 0; i < ret.size() - 1; i++) {
    auto cur_last = (ret)[i].back();
    auto next_first = (ret)[i + 1].front();
    EXPECT_TRUE(cur_last.m_height <= next_first.m_height);
  }
  for (auto &row : ret) {
    for (size_t i = 0; i < row.size() - 1; i++) {
      EXPECT_TRUE(row[i].m_height <= row[i + 1].m_height);
    }
  }
  ret.pop_back();
  for (auto &ele : ret) {
    EXPECT_EQ(ele.size(), 128);
  }

  // block interval 100
  txs.clear();
  txs_size = dis(mt);
  for (int32_t i = 0; i < txs_size; i++) {
    tx.m_height = i;
    txs.push_back(tx);
  }
  neb::block_height_t block_interval = 100;
  ret = algo_ptr->split_transactions_by_block_interval(txs, block_interval);
  actual_txs_size = 0;
  for (auto &ele : ret) {
    actual_txs_size += ele.size();
  }
  EXPECT_EQ(txs_size, actual_txs_size);
  for (size_t i = 0; i < ret.size() - 1; i++) {
    auto cur_last = (ret)[i].back();
    auto next_first = (ret)[i + 1].front();
    EXPECT_TRUE(cur_last.m_height <= next_first.m_height);
  }
  for (auto &row : ret) {
    for (size_t i = 0; i < row.size() - 1; i++) {
      EXPECT_TRUE(row[i].m_height <= row[i + 1].m_height);
    }
  }
  ret.pop_back();
  for (auto &ele : ret) {
    EXPECT_EQ(ele.size(), block_interval);
  }

  // block interval random
  txs.clear();
  txs_size = dis(mt);
  for (int32_t i = 0; i < txs_size; i++) {
    tx.m_height = i;
    txs.push_back(tx);
  }
  block_interval = std::sqrt(txs_size);
  ret = algo_ptr->split_transactions_by_block_interval(txs, block_interval);
  actual_txs_size = 0;
  for (auto &ele : ret) {
    actual_txs_size += ele.size();
  }
  EXPECT_EQ(txs_size, actual_txs_size);
  for (size_t i = 0; i < ret.size() - 1; i++) {
    auto cur_last = (ret)[i].back();
    auto next_first = (ret)[i + 1].front();
    EXPECT_TRUE(cur_last.m_height <= next_first.m_height);
  }
  for (auto &row : ret) {
    for (size_t i = 0; i < row.size() - 1; i++) {
      EXPECT_TRUE(row[i].m_height <= row[i + 1].m_height);
    }
  }
  ret.pop_back();
  for (auto &ele : ret) {
    EXPECT_EQ(ele.size(), block_interval);
  }
}

TEST(test_runtime_nebulas_rank, filter_empty_transactions_this_interval) {
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  std::vector<std::vector<neb::fs::transaction_info_t>> txs;
  algo_ptr->filter_empty_transactions_this_interval(txs);
  EXPECT_TRUE(txs.empty());

  std::vector<neb::fs::transaction_info_t> tx;
  txs.push_back(tx);
  EXPECT_TRUE(!txs.empty());
  algo_ptr->filter_empty_transactions_this_interval(txs);
  EXPECT_TRUE(txs.empty());

  neb::fs::transaction_info_t info;
  tx.push_back(info);
  txs.push_back(tx);
  EXPECT_TRUE(!txs.empty());
  algo_ptr->filter_empty_transactions_this_interval(txs);
  EXPECT_TRUE(!txs.empty());

  txs.clear();
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, 1 << 12);

  int32_t empty_ratio = dis(mt);
  int32_t empty_size = 0;
  int32_t not_empty_size = 0;
  int32_t txs_size = dis(mt);

  for (int32_t i = 0; i < txs_size; i++) {
    auto tmp = dis(mt);
    if (tmp < empty_ratio) {
      std::vector<neb::fs::transaction_info_t> tx;
      txs.push_back(tx);
      empty_size++;
    } else {
      neb::fs::transaction_info_t info;
      std::vector<neb::fs::transaction_info_t> tx;
      tx.push_back(info);
      txs.push_back(tx);
      not_empty_size++;
    }
  }
  EXPECT_EQ(txs_size, empty_size + not_empty_size);
  algo_ptr->filter_empty_transactions_this_interval(txs);
  EXPECT_EQ(txs.size(), not_empty_size);
}

std::unique_ptr<std::vector<neb::fs::transaction_info_t>>
gen_transactions(std::vector<int32_t> &addr_set) {
  size_t ch_size = 'z' - 'a';
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, ch_size);
  size_t txs_size = dis(mt) * dis(mt);
  txs_size++;
  addr_set = std::vector<int32_t>(ch_size + 1);

  auto ret = std::make_unique<std::vector<neb::fs::transaction_info_t>>();
  for (size_t i = 0; i < txs_size; i++) {
    char ch_s = 'a' + dis(mt);
    char ch_t = 'a' + dis(mt);

    addr_set[ch_s - 'a'] = 1;
    addr_set[ch_t - 'a'] = 1;

    neb::fs::transaction_info_t info;
    info.m_height = dis(mt);
    info.m_from = neb::to_address(std::string(1, ch_s));
    info.m_to = neb::to_address(std::string(1, ch_t));
    info.m_tx_value = dis(mt);
    info.m_timestamp = dis(mt);
    ret->push_back(info);
  }
  return ret;
}

bool check_for_transaction_graph(
    const neb::rt::transaction_graph_ptr_t &tg_ptr,
    const std::vector<neb::fs::transaction_info_t> &txs) {

  auto g = tg_ptr->internal_graph();
  neb::rt::transaction_graph::viterator_t vi, vi_end;
  for (boost::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; vi++) {
    neb::rt::transaction_graph::oeiterator_t oei, oei_end;
    for (boost::tie(oei, oei_end) = boost::out_edges(*vi, g); oei != oei_end;
         oei++) {
      auto source = boost::source(*oei, g);
      auto target = boost::target(*oei, g);
      auto ss = neb::to_address(boost::get(boost::vertex_name_t(), g, source));
      auto tt = neb::to_address(boost::get(boost::vertex_name_t(), g, target));
      neb::wei_t w = boost::get(boost::edge_weight_t(), g, *oei);
      int64_t ts = boost::get(boost::edge_timestamp_t(), g, *oei);

      for (auto &tx : txs) {
        if (ss == tx.m_from && tt == tx.m_to && w == tx.m_tx_value &&
            ts == tx.m_timestamp) {
          return true;
        }
      }
    }
  }
  return false;
}

TEST(test_runtime_nebulas_rank, build_graph_from_transactions) {
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto friend_ptr = std::make_unique<neb::rt::nr::friend_nebulas_rank_algo>();
  std::vector<int32_t> addr_set;
  auto txs_ptr = gen_transactions(addr_set);
  auto ret =
      friend_ptr->build_graph_from_transactions(algo_ptr.get(), *txs_ptr);
  bool is_found = check_for_transaction_graph(std::move(ret), *txs_ptr);
  EXPECT_TRUE(is_found);
}

TEST(test_runtime_nebulas_rank, build_transaction_graphs) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, 1 << 10);
  int32_t txs_v_size = dis(mt);

  std::vector<std::vector<neb::fs::transaction_info_t>> txs_v;
  for (int32_t i = 0; i < txs_v_size; i++) {
    std::vector<int32_t> addr_set;
    auto txs_ptr = gen_transactions(addr_set);
    txs_v.push_back(*txs_ptr);
  }
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto ret = algo_ptr->build_transaction_graphs(txs_v);
  auto &g_v = ret;

  for (int32_t i = 0; i < txs_v_size; i++) {
    bool is_found = check_for_transaction_graph(std::move(g_v[i]), txs_v[i]);
    EXPECT_TRUE(is_found);
  }
}

TEST(test_runtime_nebulas_rank, get_max_height_this_block_interval) {
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto friend_ptr = std::make_unique<neb::rt::nr::friend_nebulas_rank_algo>();
  std::vector<neb::fs::transaction_info_t> txs;
  auto ret =
      friend_ptr->get_max_height_this_block_interval(algo_ptr.get(), txs);
  EXPECT_EQ(0, ret);

  std::vector<int32_t> addr_set;
  auto tmp = gen_transactions(addr_set);
  txs = *tmp;
  std::sort(txs.begin(), txs.end(),
            [](const neb::fs::transaction_info_t &info1,
               const neb::fs::transaction_info_t &info2) {
              return info1.m_height < info2.m_height;
            });
  ret = friend_ptr->get_max_height_this_block_interval(algo_ptr.get(), txs);
  EXPECT_EQ(ret, txs.back().m_height);
}

TEST(test_runtime_nebulas_rank, get_normal_accounts) {
  std::vector<int32_t> addr_set;
  auto txs_ptr = gen_transactions(addr_set);
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto ret = algo_ptr->get_normal_accounts(*txs_ptr);

  int32_t addr_sum = 0;
  for (size_t i = 0; i < addr_set.size(); i++) {
    if (addr_set[i]) {
      char ch = 'a' + i;
      auto addr = neb::to_address(std::string(1, ch));
      EXPECT_TRUE(ret.find(addr) != ret.end());
      addr_sum += addr_set[i];
    }
  }
  EXPECT_EQ(addr_sum, ret.size());
}

TEST(test_runtime_nebulas_rank, f_account_weight) {
  auto tmp_func = [](float in_val, float out_val) {
    float pi = std::acos(-1.0);
    float atan_val = (in_val == 0 ? pi / 2 : std::atan(out_val / in_val));
    float in_out_sum = in_val + out_val;
    float tmp = std::sin(pi / 4.0 - atan_val);
    float ret = in_out_sum * std::exp((-2.0) * tmp * tmp);
    return ret;
  };

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, std::numeric_limits<int16_t>::max());

  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto friend_ptr = std::make_unique<neb::rt::nr::friend_nebulas_rank_algo>();

  for (int32_t i = 0; i < 1000; i++) {
    auto in_val = dis(mt);
    auto out_val = dis(mt);
    auto expect_ret = tmp_func(in_val, out_val);
    auto actual_ret =
        friend_ptr->f_account_weight(algo_ptr.get(), in_val, out_val);
    EXPECT_TRUE(neb::math::abs(actual_ret, neb::floatxx_t(expect_ret)) <
                precesion(expect_ret, 1e1));
  }
}

TEST(test_runtime_nebulas_rank, f_accout_rank) {
  auto tmp_func = [](int64_t a, int64_t b, int64_t c, int64_t d, float theta,
                     float mu, float lambda, float S, float R) {
    auto gamma = std::pow(theta * R / (R + mu), lambda);
    auto ret = (S / (1.0 + std::pow(a / S, 1.0 / b))) * gamma;
    return ret;
  };

  int64_t a = 100;
  int64_t b = 2;
  int64_t c = 6;
  int64_t d = -9;
  float theta = 1.0;
  float mu = 1.0;
  float lambda = 2.0;

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, std::numeric_limits<int32_t>::max());

  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto friend_ptr = std::make_unique<neb::rt::nr::friend_nebulas_rank_algo>();

  for (int32_t i = 0; i < 10; i++) {
    auto S = dis(mt);
    auto R = dis(mt);
    auto expect_ret = tmp_func(a, b, c, d, theta, mu, lambda, S, R);
    auto actual_ret = friend_ptr->f_account_rank(algo_ptr.get(), a, b, c, d,
                                                 theta, mu, lambda, S, R);
    EXPECT_TRUE(neb::math::abs(actual_ret, neb::floatxx_t(expect_ret)) <
                precesion(expect_ret, 1e2));
  }
}

TEST(test_runtime_nebulas_rank, get_account_balance_median) {
  std::unordered_set<neb::address_t> accounts;
  std::vector<std::vector<neb::fs::transaction_info_t>> txs;

  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto bc_ptr = std::make_unique<neb::fs::blockchain>(ms_ptr.get());
  auto ba_ptr = std::make_unique<neb::fs::blockchain_api>(bc_ptr.get());
  auto db_ptr = std::make_unique<neb::fs::account_db>(ba_ptr.get());
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto ret =
      algo_ptr->get_account_balance_median(1, accounts, txs, db_ptr.get());
  EXPECT_TRUE(ret.empty());
}

TEST(test_runtime_nebulas_rank, get_account_weight) {
  std::unordered_map<neb::address_t, neb::rt::in_out_val_t> in_out_vals;
  auto ms_ptr = std::make_unique<neb::fs::memory_storage>();
  auto bc_ptr = std::make_unique<neb::fs::blockchain>(ms_ptr.get());
  auto ba_ptr = std::make_unique<neb::fs::blockchain_api>(bc_ptr.get());
  auto db_ptr = std::make_unique<neb::fs::account_db>(ba_ptr.get());
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto ret = algo_ptr->get_account_weight(in_out_vals, db_ptr.get());
  EXPECT_TRUE(ret.empty());
}

TEST(test_runtime_nebulas_rank, get_account_rank) {
  std::unordered_map<neb::address_t, neb::floatxx_t> acc_m;
  std::unordered_map<neb::address_t, neb::floatxx_t> acc_w;
  neb::rt::nr::rank_params_t rp;
  auto algo_ptr = std::make_unique<neb::rt::nr::nebulas_rank_algo>();
  auto ret = algo_ptr->get_account_rank(acc_m, acc_w, rp);
  EXPECT_TRUE(ret.empty());

  acc_m.insert(std::make_pair(neb::to_address("a"), neb::floatxx_t(1)));
  ret = algo_ptr->get_account_rank(acc_m, acc_w, rp);
  EXPECT_TRUE(ret.empty());

  acc_m.clear();
  acc_w.insert(std::make_pair(neb::to_address("a"), neb::floatxx_t(1)));
  ret = algo_ptr->get_account_rank(acc_m, acc_w, rp);
  EXPECT_TRUE(ret.empty());

  rp = neb::rt::nr::rank_params_t{
      100, 2, 6, -9, neb::floatxx_t(1), neb::floatxx_t(1), neb::floatxx_t(2)};
  acc_m.insert(std::make_pair(neb::to_address("a"), neb::floatxx_t(1)));
  ret = algo_ptr->get_account_rank(acc_m, acc_w, rp);
  EXPECT_TRUE(!ret.empty());
  EXPECT_EQ(ret.size(), 1);
  EXPECT_EQ(ret.begin()->first, neb::to_address("a"));

  auto friend_ptr = std::make_unique<neb::rt::nr::friend_nebulas_rank_algo>();
  auto tmp = friend_ptr->f_account_rank(
      algo_ptr.get(), 100, 2, 6, -9, neb::floatxx_t(1), neb::floatxx_t(1),
      neb::floatxx_t(2), neb::floatxx_t(1), neb::floatxx_t(1));
  EXPECT_TRUE(ret.begin()->second == tmp);
}

