#include "metadata/metadatamanager.hpp"
#include "record/schema.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <stdexcept>
#include <string>

namespace simpledb {
TEST(metadata, metadatamanagertest2) {
  SimpleDB db = SimpleDB("metadatamanagertest2", 400, 8);
  auto tx = db.NewTx();
  MetadataManager mdm = MetadataManager(true, tx.get());
  Schema sch;
  sch.AddIntField("A");
  sch.AddStringField("B", 9);

  mdm.CreateTable("MyTable", sch, tx.get());

  /// Should raise if the table doesn't exist
  EXPECT_THROW(mdm.GetLayout("NotExistingTable", tx.get()), std::runtime_error);
  std::cout << "Did not find table NotExistingTable in the database"
            << std::endl;
  tx->Rollback();
}
} // namespace simpledb
