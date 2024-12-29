#include <iostream>
#include <memory>
#include <random>

#include "query/expression.hpp"
#include "query/predicate.hpp"
#include "query/productscan.hpp"
#include "query/projectscan.hpp"
#include "query/selectscan.hpp"
#include "query/updatescan.hpp"
#include "record/layout.hpp"
#include "record/tablescan.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"

namespace simpledb {
TEST(query, scantest) {
  SimpleDB db("scantest", 400, 8);
  auto tx = db.NewTx();

  Schema sch1;
  sch1.AddIntField("A");
  sch1.AddStringField("B", 9);
  Layout layout(sch1);
  auto scan1 = std::static_pointer_cast<UpdateScan>(
      std::make_shared<TableScan>(tx.get(), "T", layout));

  scan1->BeforeFirst();
  int n = 200;
  std::cout << "Inserting " << n << " random records" << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> d(0, 1);

  for (int i = 0; i < n; i++) {
    scan1->Insert();
    int k = round(d(gen) * 50);
    scan1->SetInt("A", k);
    scan1->SetString("B", "rec" + std::to_string(k));
  }
  scan1->Close();

  auto scan2 = std::static_pointer_cast<Scan>(
      std::make_shared<TableScan>(tx.get(), "T", layout));
  Constant c(10);
  Term t(Expression("A"), Expression(c));
  Predicate p(t);

  std::cout << "The predicate is: " << p.ToString() << std::endl;

  // Pipelined Query Processing:
  //
  // Now we scan to find the predicate. First use a select scan to find the
  // predicate. Then pass the result of the select scan to the project scan to
  // get just field B.
  auto scan3 =
      std::static_pointer_cast<Scan>(std::make_shared<SelectScan>(scan2, p));
  std::vector<std::string> fields = {"B"};
  auto scan4 = std::static_pointer_cast<Scan>(
      std::make_shared<ProjectScan>(scan3, fields));
  while (scan4->Next()) {
    std::cout << scan4->GetString("B") << std::endl;
  }

  scan4->Close();
  tx->Commit();
}

TEST(query, scantest2) {
  SimpleDB db("scantest2", 400, 8);
  auto tx = db.NewTx();

  Schema sch1;
  sch1.AddIntField("A");
  sch1.AddStringField("B", 9);
  Layout layout(sch1);
  auto scan1 = std::static_pointer_cast<UpdateScan>(
      std::make_shared<TableScan>(tx.get(), "T1", layout));

  scan1->BeforeFirst();
  int n = 200;
  std::cout << "Inserting " << n << " records into T1." << std::endl;

  for (int i = 0; i < n; i++) {
    scan1->Insert();
    scan1->SetInt("A", i);
    scan1->SetString("B", "rec" + std::to_string(i));
  }
  scan1->Close();

  Schema sch2;
  sch2.AddIntField("C");
  sch2.AddStringField("D", 9);
  Layout layout2(sch2);

  auto scan2 = std::static_pointer_cast<UpdateScan>(
      std::make_shared<TableScan>(tx.get(), "T2", layout2));

  scan2->BeforeFirst();
  std::cout << "Inserting " << n << " records into T2." << std::endl;
  for (int i = 0; i < n; i++) {
    scan2->Insert();
    scan2->SetInt("C", n - i - 1);
    scan2->SetString("D", "rec" + std::to_string(n - i - 1));
  }
  scan2->Close();

  /// Now scan the tables and get the cross product
  auto s1 = std::static_pointer_cast<Scan>(
      std::make_shared<TableScan>(tx.get(), "T1", layout));
  auto s2 = std::static_pointer_cast<Scan>(
      std::make_shared<TableScan>(tx.get(), "T2", layout2));

  auto s3 =
      std::static_pointer_cast<Scan>(std::make_shared<ProductScan>(s1, s2));

  /// Select just the records where A=C
  Term t(Expression("A"), Expression("C"));
  Predicate p(t);
  std::cout << "The predicate is: " << p.ToString() << std::endl;
  auto s4 = std::static_pointer_cast<Scan>(std::make_shared<SelectScan>(s3, p));

  /// Project just fields B and D
  std::vector<std::string> fields = {"B", "D"};
  auto s5 =
      std::static_pointer_cast<Scan>(std::make_shared<ProjectScan>(s4, fields));

  while (s5->Next()) {
    std::cout << s5->GetString("B") << " " << s5->GetString("D") << std::endl;
  }

  s5->Close();
  tx->Commit();
}

} // namespace simpledb
