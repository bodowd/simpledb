#pragma once

#include "buffer/buffermanager.hpp"
#include "log/logmanager.hpp"

namespace simpledb {
// a forward declaration to let the compiler know that this is a valid
// type without including the entire defintion of Transaction class
class Transaction;
class RecoveryManager {

public:
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
