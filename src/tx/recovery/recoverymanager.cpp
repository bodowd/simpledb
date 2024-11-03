#include "tx/recovery/recoverymanager.hpp"
#include "tx/transaction.hpp"

namespace simpledb {
RecoveryManager::RecoveryManager(Transaction *tx, int txnum, LogManager *lm,
                                 BufferManager *bm) {
  _tx = tx;
  _txnum = txnum;
  _log_manager = lm;
  _buffer_manager = bm;
};
} // namespace simpledb
