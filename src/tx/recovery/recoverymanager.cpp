#include "tx/recovery/recoverymanager.hpp"
#include "tx/recovery/logrecord.hpp"
#include "tx/transaction.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

namespace simpledb {
RecoveryManager::RecoveryManager(Transaction *tx, int txnum, LogManager *lm,
                                 BufferManager *bm) {
  _tx = tx;
  _txnum = txnum;
  _log_manager = lm;
  _buffer_manager = bm;
  StartRecord::WriteToLog(_log_manager, _txnum);
};

// Since we implement undo-only recovery, we need to
// flush the writes before writing to the log. This way the recovery manager
// is sure that all committed modifications have been written to disk. We
// then only need to undo uncompleted (no commit or rollback log record)
// transactions
//
// 1. flush the modified buffers
// 2. write commit record to log
// 3. flush the log
void RecoveryManager::Commit() {
  _buffer_manager->FlushAll(_txnum);
  int lsn = CommitRecord::WriteToLog(_log_manager, _txnum);
  _log_manager->Flush(lsn);
}

void RecoveryManager::Rollback() {
  doRollback();
  _buffer_manager->FlushAll(_txnum);
  int lsn = RollbackRecord::WriteToLog(_log_manager, _txnum);
  _log_manager->Flush(lsn);
}

void RecoveryManager::Recover() {
  doRecover();
  _buffer_manager->FlushAll(_txnum);
  int lsn = CheckpointRecord::WriteToLog(_log_manager);
  _log_manager->Flush(lsn);
}

int RecoveryManager::SetInt(Buffer *buff, int offset, int newval) {
  int oldval = buff->Contents()->GetInt(offset);
  BlockId blk = buff->Block();
  return SetIntRecord::WriteToLog(_log_manager, _txnum, blk, offset, oldval);
}

int RecoveryManager::SetString(Buffer *buff, int offset,
                               const std::string &newval) {
  std::string oldval = buff->Contents()->GetString(offset);
  BlockId blk = buff->Block();
  return SetStringRecord::WriteToLog(_log_manager, _txnum, blk, offset, oldval);
}

void RecoveryManager::doRollback() {
  auto iter = _log_manager->Iterator();
  while (iter.HasNext()) {
    std::vector<char> bytes = iter.Next();
    auto rec = LogRecord::CreateLogRecord(bytes);
    if (rec->TxNumber() == _txnum) {
      if (rec->Op() == LogRecord::start) {
        return;
      } else {
        rec->Undo(_tx);
      }
    }
  }
}

// Implements Undo-only recovery.
// Do backward pass through the log and gather the committed transactions. Since
// we flush committed transactions before writing to the log, if we see the
// "committed" log record, we know the transaction has completed.
void RecoveryManager::doRecover() {
  std::vector<int> finishedTxns;
  auto iter = _log_manager->Iterator();
  while (iter.HasNext()) {
    std::vector<char> bytes = iter.Next();
    auto rec = LogRecord::CreateLogRecord(bytes);
    if (rec->Op() == LogRecord::checkpoint) {
      return;
    }
    if (rec->Op() == LogRecord::commit || rec->Op() == LogRecord::rollback) {
      finishedTxns.emplace_back(rec->TxNumber());
      // if std::find returns finishedTxns.end() it means it did not find this
      // TxNumber. That means this TxNumber was not a committed transaction
    } else if (std::find(finishedTxns.begin(), finishedTxns.end(),
                         rec->TxNumber()) == finishedTxns.end()) {
      rec->Undo(_tx);
    }
  }
}

} // namespace simpledb
