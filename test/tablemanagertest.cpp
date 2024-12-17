#include "metadata/tablemangager.hpp"
#include "record/schema.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
namespace simpledb {
TEST(metadata, tablemanagertest) {
  SimpleDB db = SimpleDB("tablemanagertest", 400, 8);
  auto tx = db.NewTx();
  TableManager tm = TableManager(true, tx.get());

  Schema sch;
  sch.AddIntField("A");
  sch.AddStringField("B", 9);
  tm.CreateTable("MyTable", sch, tx.get());

  Layout layout = tm.GetLayout("MyTable", tx.get());
  int size = layout.SlotSize();
  Schema sch2 = layout.GetSchema();
  std::cout << "MyTable has slot slize " << size << std::endl;
  std::cout << "Its fields are: " << std::endl;
  for (const std::string &fieldname : sch2.Fields()) {
    std::string type;
    if (sch2.Type(fieldname) == Schema::INTEGER) {
      type = "int";
    } else {
      int strlen = sch2.Length(fieldname);
      type = "VARCHAR(" + std::to_string(strlen) + ")";
    }
    std::cout << fieldname << ": " << type << std::endl;
  }

  tx->Commit();
}

} // namespace simpledb
