#include "metadata/tablemangager.hpp"
#include "record/schema.hpp"
#include "record/tablescan.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
namespace simpledb {
TEST(metadata, catalogtest) {
  SimpleDB db("catalogtest", 400, 8);
  auto tx = db.NewTx();
  TableManager tm(true, tx.get());

  Schema sch;
  sch.AddIntField("A");
  sch.AddStringField("B", 9);
  tm.CreateTable("MyTable", sch, tx.get());

  Schema sch2;
  sch2.AddIntField("C");
  sch2.AddStringField("D", 9);
  tm.CreateTable("SecondTable", sch2, tx.get());

  std::cout << "All tables and their lengths" << std::endl;

  Layout lt = tm.GetLayout(tm.TABLECAT, tx.get());
  TableScan ts = TableScan(tx.get(), tm.TABLECAT, lt);

  while (ts.Next()) {
    std::string tname = ts.GetString(tm.TABLENAME);
    int size = ts.GetInt(tm.SLOTSIZE);
    std::cout << tname << " " << size << std::endl;
  }
  ts.Close();

  std::cout << "All fields and their offsets" << std::endl;
  Layout layout2 = tm.GetLayout(tm.FIELDCAT, tx.get());
  TableScan ts2 = TableScan(tx.get(), tm.FIELDCAT, layout2);
  while (ts2.Next()) {
    std::string tname = ts2.GetString(tm.TABLENAME);
    std::string fname = ts2.GetString("fldname");
    int offset = ts2.GetInt("offset");
    std::cout << tname << " " << fname << " " << offset << std::endl;
    ;
  }

  ts2.Close();
  tx->Commit();
}
} // namespace simpledb
