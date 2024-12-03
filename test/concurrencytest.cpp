#include "buffer/buffermanager.hpp"
#include "file/blockid.hpp"
#include "file/filemanager.hpp"
#include "log/logmanager.hpp"
#include "server/simpledb.hpp"
#include "tx/transaction.hpp"
#include "gtest/gtest.h"
#include <memory>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

namespace simpledb {
void run_A(FileManager &fm, LogManager &lm, BufferManager &bm) {
  try {
    auto txA = std::make_unique<Transaction>(&fm, &lm, &bm);
    std::string f = "testfile";
    BlockId blk1 = BlockId(f, 1);
    BlockId blk2 = BlockId(f, 2);
    txA->Pin(blk1);
    txA->Pin(blk2);
    std::cout << "Tx A: request slock 1" << std::endl;
    ;
    txA->GetInt(blk1, 0);
    std::cout << "Tx A: receive slock 1" << std::endl;
    std::this_thread::sleep_for(1000ms);
    std::cout << "Tx A: request slock 2" << std::endl;
    txA->GetInt(blk2, 0);
    std::cout << "Tx A: receive slock 2" << std::endl;
    txA->Commit();
    std::cout << "Tx A: commit" << std::endl;

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

void run_B(FileManager &fm, LogManager &lm, BufferManager &bm) {
  try {
    auto txB = std::make_unique<Transaction>(&fm, &lm, &bm);
    std::string f = "testfile";
    BlockId blk1 = BlockId(f, 1);
    BlockId blk2 = BlockId(f, 2);
    txB->Pin(blk1);
    txB->Pin(blk2);
    std::cout << "Tx B: request xlock 2" << std::endl;
    txB->SetInt(blk2, 0, 0, false);
    std::cout << "Tx B: receive xlock 2" << std::endl;
    std::this_thread::sleep_for(1000ms);
    std::cout << "Tx B: request slock 1" << std::endl;
    txB->GetInt(blk1, 0);
    std::cout << "Tx B: receive slock 1" << std::endl;
    txB->Commit();
    std::cout << "Tx B: commit" << std::endl;

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

void run_C(FileManager &fm, LogManager &lm, BufferManager &bm) {
  try {
    auto txC = std::make_unique<Transaction>(&fm, &lm, &bm);
    std::string f = "testfile";
    BlockId blk1 = BlockId(f, 1);
    BlockId blk2 = BlockId(f, 2);
    txC->Pin(blk1);
    txC->Pin(blk2);
    std::this_thread::sleep_for(500ms); // this seems necessary. without it
                                        // seems like there's a deadlock??
    std::cout << "Tx C: request xlock 1" << std::endl;
    txC->SetInt(blk1, 0, 0, false);
    std::cout << "Tx C: receive xlock 1" << std::endl;
    std::this_thread::sleep_for(1000ms);
    std::cout << "Tx C: request slock 2" << std::endl;
    txC->GetInt(blk2, 0);
    std::cout << "Tx C: receive slock 2" << std::endl;
    txC->Commit();
    std::cout << "Tx C: commit" << std::endl;

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}

TEST(transaction, concurrency_test) {
  SimpleDB db = SimpleDB("concurrencytest", 400, 8);
  FileManager &fm = db.GetFileManager();
  LogManager &lm = db.GetLogManager();
  BufferManager &bm = db.GetBufferManager();

  std::thread A(run_A, std::ref(fm), std::ref(lm), std::ref(bm));
  std::thread B(run_B, std::ref(fm), std::ref(lm), std::ref(bm));
  std::thread C(run_C, std::ref(fm), std::ref(lm), std::ref(bm));

  A.join();
  B.join();
  C.join();
}
} // namespace simpledb
