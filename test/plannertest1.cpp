#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <random>

namespace simpledb {
TEST(plan, plannertest1_test) {
  SimpleDB db("plannertest1");
  auto tx = db.NewTx();
  Planner &planner = db.GetPlanner();
  std::string cmd = "create table T1(A int, B varchar(9))";
  planner.ExecuteUpdate(cmd, tx.get());

  int n = 200;
  std::cout << "Inserting " << n << " random records" << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> d(0, 1);

  for (int i = 0; i < n; i++) {
    int a = round(d(gen) * 50);
    std::string b = "rec" + std::to_string(a);
    cmd = "insert into T1(A,B) values (" + std::to_string(a) + ", '" + b + "')";
    planner.ExecuteUpdate(cmd, tx.get());
  }

  std::string query = "select B from T1 where A=10";
  auto plan = planner.CreateQueryPlan(query, tx.get());
  auto scan = plan->Open();
  while (scan->Next()) {
    std::cout << scan->GetString("b") << std::endl;
  }
  scan->Close();
  tx->Commit();
}
} // namespace simpledb
