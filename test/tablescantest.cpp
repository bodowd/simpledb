#include "record/tablescan.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <random>
#include <string>

namespace simpledb {
TEST(record, tablescantest) {
  SimpleDB db = SimpleDB("tabletest", 400, 8);
  auto tx = db.NewTx();
  Schema sch;
  sch.AddIntField("A");
  sch.AddStringField("B", 9);
  Layout layout(sch);
  for (const std::string &fieldname : layout.GetSchema().Fields()) {
    int offset = layout.Offset(fieldname);
    std::cout << fieldname << " has offset " << offset << std::endl;
  }

  TableScan ts = TableScan(tx.get(), "T", layout);
  std::cout << "Filling the table with 50 random records." << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> d(0, 1);

  ts.BeforeFirst();
  int countLessThan = 0;
  for (int i = 0; i < 50; i++) {
    ts.Insert();
    int n = round(d(gen) * 50);
    ts.SetInt("A", n);
    if (n < 25) {
      countLessThan++;
    }
    ts.SetString("B", "rec" + std::to_string(n));
    std::cout << "inserting into slot " << ts.GetRid().ToString() << ": {" << n
              << ", rec" << n << "}" << std::endl;
  }

  std::cout << "Deleting records with A-values < 25." << std::endl;
  int count = 0;
  ts.BeforeFirst();
  while (ts.Next()) {
    int a = ts.GetInt("A");
    std::string b = ts.GetString("B");
    if (a < 25) {
      count++;
      std::cout << "slot " << ts.GetRid().ToString() << ": {" << a << ", " << b
                << "}" << std::endl;
      ts.Delete();
    }
  }
  ASSERT_EQ(count, countLessThan);

  std::cout << count << " values under 25 were deleted" << std::endl;
  std::cout << "Here are the remaining records." << std::endl;

  ts.BeforeFirst();
  while (ts.Next()) {
    int a = ts.GetInt("A");
    std::string b = ts.GetString("B");
    std::cout << "slot " << ts.GetRid().ToString() << ": {" << a << ", " << b
              << "}" << std::endl;
  }

  ts.Close();
  tx->Commit();
}
} // namespace simpledb
