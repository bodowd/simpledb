#include "metadata/metadatamanager.hpp"
#include "record/schema.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
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

  /// Part 3: View Metadata
  std::string viewdef = "select B from MyTable where A = 1";
  mdm.CreateView("viewA", viewdef, tx.get());
  std::string v = mdm.GetViewDef("viewA", tx.get());
  std::cout << "View def = " << v << std::endl;
  ASSERT_EQ(v, viewdef);

  tx->Commit();
}

} // namespace simpledb
