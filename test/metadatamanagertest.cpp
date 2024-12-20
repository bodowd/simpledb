#include "metadata/metadatamanager.hpp"
#include "record/schema.hpp"
#include "record/tablescan.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <random>
#include <string>
namespace simpledb {
TEST(metadata, metadatamanagertest) {
  SimpleDB db = SimpleDB("metadatamanagertest", 400, 8);
  auto tx = db.NewTx();
  MetadataManager mdm = MetadataManager(true, tx.get());

  Schema sch;
  sch.AddIntField("A");
  sch.AddStringField("B", 9);

  /// Part 1: Table Metadata
  mdm.CreateTable("MyTable", sch, tx.get());
  Layout layout = mdm.GetLayout("MyTable", tx.get());
  int size = layout.SlotSize();
  Schema sch2 = layout.GetSchema();
  std::cout << "MyTable has slot size " << size << std::endl;
  std::cout << "Its fields are:" << std::endl;
  for (const std::string &fieldname : sch2.Fields()) {
    std::string type;
    if (sch2.Type(fieldname) == Schema::INTEGER) {
      type = "int";
    } else {
      int strlen = sch2.Length(fieldname);
      type = "varchar(" + std::to_string(strlen) + ")";
    }
    std::cout << fieldname << ": " << type << std::endl;
  }

  ASSERT_EQ(sch2.Fields().size(), 2);
  EXPECT_EQ(sch2.Type("A"), Schema::INTEGER);
  EXPECT_EQ(sch2.Type("B"), Schema::VARCHAR);
  EXPECT_EQ(sch2.Length("B"), 9);

  /// Part 2: Statistics Metadata
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> d(0, 1);

  TableScan ts(tx.get(), "MyTable", layout);
  for (int i = 0; i < 50; i++) {
    ts.Insert();
    int n = round(d(gen) * 50);
    ts.SetInt("A", n);
    ts.SetString("B", "rec" + std::to_string(n));
  }

  StatInfo si = mdm.GetStatInfo("MyTable", layout, tx.get());
  std::cout << "Blocks accessed: " << si.BlocksAccessed() << std::endl;
  std::cout << "Number of records in table: " << si.RecordsOutput()
            << std::endl;
  ASSERT_EQ(si.RecordsOutput(), 50);
  std::cout << "Distinct values in field A: " << si.DistinctValues("A")
            << std::endl;

  /// Part 3: View Metadata
  std::string viewdef = "select B from MyTable where A = 1";
  mdm.CreateView("viewA", viewdef, tx.get());
  std::string v = mdm.GetViewDef("viewA", tx.get());
  std::cout << "View def = " << v << std::endl;
  ASSERT_EQ(v, viewdef);

  tx->Commit();
}

} // namespace simpledb
