#include "tx/transaction.hpp"
#include "file/blockid.hpp"
#include "file/filemanager.hpp"
#include "log/logmanager.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <memory>

/*
 * This test consists of 4 transactions. All four transactions access block 1 of
 * the test file. tx1 intializes the values at offsets 80 and 40.
 * Tx2 reads those values, prints them, and increments them.
 * Tx3 reads and prints the incremented values. Then it sets the integer to 9999
 * and rolls back.
 * Tx4 reads the integer to verify that the rollback occurred
 */
namespace simpledb {
TEST(transaction, transaction_test) {
  SimpleDB db = SimpleDB("txtest", 400, 8);
  // std::unique_ptr<FileManager> fm = db.GetFileManager();
  // std::unique_ptr<LogManager> lm = db.GetLogManager();
  FileManager &fm = db.GetFileManager();
  LogManager &lm = db.GetLogManager();
  auto &bm = db.GetBufferManager();

  auto tx1 = std::make_unique<Transaction>(&fm, &lm, &bm);
  BlockId blk = BlockId("testfile", 1);
  tx1->Pin(blk);

  // the block initially contains unknown bytes so do not log it
  tx1->SetInt(blk, 80, 1, false);
  tx1->SetString(blk, 40, "one", false);
  tx1->Commit();

  auto tx2 = std::make_unique<Transaction>(&fm, &lm, &bm);
  tx2->Pin(blk);
  int ival = tx2->GetInt(blk, 80);
  std::string sval = tx2->GetString(blk, 40);
  std::cout << "initial value at location 80: " << ival << std::endl;
  std::cout << "initial value at location 40: " << sval << std::endl;
  int newIval = ival + 1;
  std::string newSval = sval + "!";
  tx2->SetInt(blk, 80, newIval, true);
  tx2->SetString(blk, 40, newSval, true);
  tx2->Commit();

  auto tx3 = std::make_unique<Transaction>(&fm, &lm, &bm);
  tx3->Pin(blk);
  std::cout << "new value at location 80: " << tx3->GetInt(blk, 80)
            << std::endl;
  std::cout << "new value at location 40: " << tx3->GetString(blk, 40)
            << std::endl;
  tx3->SetInt(blk, 80, 9999, true);
  std::cout << "pre-rollback value at location 80: " << tx3->GetInt(blk, 80)
            << std::endl;
  tx3->Rollback();

  // since the previous transaction was rolled back, the new transaction should
  // not see the modification from transaction 3, and it should see what was
  // committed before by transaction 2
  auto tx4 = std::make_unique<Transaction>(&fm, &lm, &bm);
  tx4->Pin(blk);
  std::cout << "post-rollback at location 80: " << tx4->GetInt(blk, 80)
            << std::endl;
  tx4->Commit();
}

} // namespace simpledb
