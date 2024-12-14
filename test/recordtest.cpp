#include "record/recordpage.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <random>
#include <string>

/*
 * This test illustrates the use of the RecordPage methods
 * */

namespace simpledb {
TEST(record, record_test) {
  SimpleDB db = SimpleDB("recordtest", 400, 8);
  auto tx = db.NewTx();
  Schema sch;
  sch.AddIntField("A");
  sch.AddStringField("B", 9);
  Layout layout(sch);

  for (const std::string &fieldname : layout.GetSchema().Fields()) {
    int offset = layout.Offset(fieldname);
    std::cout << fieldname << " has offset " << offset << std::endl;
  }

  BlockId blk = tx->Append("testfile");
  tx->Pin(blk);

  RecordPage rp = RecordPage(tx.get(), blk, layout);
  rp.Format();

  std::cout << "Filling the page with random records..." << std::endl;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> d(0, 1);

  int slot = rp.InsertAfter(-1);
  while (slot >= 0) {
    int n = round(d(gen) * 50);
    rp.SetInt(slot, "A", n);
    rp.SetString(slot, "B", "rec" + std::to_string(n));
    std::cout << "Inserting into slot " << slot << ": {" << n << ", rec" << n
              << "}" << std::endl;
    slot = rp.InsertAfter(slot);
  }

  std::cout << "Deleted these records with A-values < 25." << std::endl;
  int count = 0;
  slot = rp.NextAfter(-1);
  while (slot >= 0) {
    int a = rp.GetInt(slot, "A");
    std::string b = rp.GetString(slot, "B");
    if (a < 25) {
      count++;
      std::cout << "slot " << slot << ": {" << a << ", " << b << "}"
                << std::endl;
      rp.Delete(slot);
    }
    slot = rp.NextAfter(slot);
  }

  std::cout << count << " values under 25 were deleted." << std::endl;
  std::cout << "Here are the remaining records." << std::endl;

  slot = rp.NextAfter(-1);
  while (slot >= 0) {
    int a = rp.GetInt(slot, "A");
    std::string b = rp.GetString(slot, "B");
    std::cout << "slot " << slot << ": {" << a << ", " << b << "}" << std::endl;
    slot = rp.NextAfter(slot);
  }
  tx->Unpin(blk);
  tx->Commit();
}
} // namespace simpledb
