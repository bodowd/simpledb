#pragma once
#include "buffer/buffermanager.hpp"
#include "log/logmanager.hpp"

namespace simpledb {
// a forward declaration to let the compiler know that this is a valid
// type without including the entire defintion of Transaction class
class Transaction;

class RecoveryManager {

public:
  // (Page 124 in the book)
  // Recovery is performed each time the database engine starts up and the log
  // is used to recover the DB.
  // 1. All uncompleted transactions should be rolled back
  // 2. All committed transactions should have their modifications written to
  // disk It makes sure the DB has the durability property in ACID.
  //
  // Each transaction has its own RecoveryManager. The RecoveryManager writes
  // the appropriate log records for that transaction.
  //
  // Implements the undo-only recovery algorithm:
  // Commit and Rollback methods flush the transaction's buffers _before_
  // writing their log record, and then doRollback and doRecovery make a single
  // backward pass through the log. Page 114 in the book details the Undo only
  // recovery algorithm
  RecoveryManager(Transaction *tx, int txnum, LogManager *lm,
                  BufferManager *bm);
  void Commit();
  void Rollback();
  void Recover();
  int SetInt(Buffer *buff, int offset, int newval);
  int SetString(Buffer *buff, int offset, const std::string &newval);

private:
  Transaction *_tx;
  int _txnum;
  LogManager *_log_manager;
  BufferManager *_buffer_manager;

  void doRollback();
  void doRecover();
};
} // namespace simpledb
