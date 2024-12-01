#include "tx/transaction.hpp"
#include "file/blockid.hpp"
#include "file/filemanager.hpp"
#include "log/logmanager.hpp"
#include "tx/recovery/recoverymanager.hpp"
#include <iostream>
#include <memory>
#include <mutex>

namespace simpledb {
int Transaction::_next_tx_num = 0;
std::mutex Transaction::_mutex;

Transaction::Transaction(FileManager *fm, LogManager *lm, BufferManager *bm) {
  _file_manager = fm;
  _log_manager = lm;
  _buffer_manager = bm;
  _tx_num = nextTxNum();

  _recovery_manager = std::make_unique<RecoveryManager>(
      this, _tx_num, _log_manager, _buffer_manager);
  _concurrency_manager = std::make_unique<ConcurrencyManager>();
  _buffer_list = std::make_unique<BufferList>(_buffer_manager);
};

void Transaction::Commit() {
  _recovery_manager->Commit();
  _concurrency_manager->Release();
  _buffer_list->UnpinAll();
  std::cout << "transaction " << _tx_num << " committed" << std::endl;
}

void Transaction::Rollback() {
  _recovery_manager->Rollback();
  _concurrency_manager->Release();
  _buffer_list->UnpinAll();
  std::cout << "transaction " << _tx_num << " rolled back" << std::endl;
  ;
}

void Transaction::Recover() {
  _buffer_manager->FlushAll(_tx_num);
  _recovery_manager->Recover();
}

void Transaction::Pin(const BlockId &blk) { _buffer_list->Pin(blk); }

void Transaction::Unpin(const BlockId &blk) { _buffer_list->Unpin(blk); }

int Transaction::GetInt(const BlockId &blk, int offset) {
  _concurrency_manager->SLock(blk);
  Buffer *buff = _buffer_list->GetBuffer(blk);
  return buff->Contents()->GetInt(offset);
}

std::string Transaction::GetString(const BlockId &blk, int offset) {
  _concurrency_manager->SLock(blk);
  Buffer *buff = _buffer_list->GetBuffer(blk);
  return buff->Contents()->GetString(offset);
}

void Transaction::SetInt(const BlockId &blk, int offset, int val,
                         bool okToLog) {
  _concurrency_manager->XLock(blk);
  Buffer *buff = _buffer_list->GetBuffer(blk);
  int lsn = -1;
  if (okToLog) {
    lsn = _recovery_manager->SetInt(buff, offset, val);
  }
  Page *p = buff->Contents();
  p->SetInt(offset, val);
  buff->SetModified(_tx_num, lsn);
}

void Transaction::SetString(const BlockId &blk, int offset, std::string val,
                            bool okToLog) {
  _concurrency_manager->XLock(blk);
  Buffer *buff = _buffer_list->GetBuffer(blk);
  int lsn = -1;
  if (okToLog) {
    lsn = _recovery_manager->SetString(buff, offset, val);
  }
  Page *p = buff->Contents();
  p->SetString(offset, val);
  buff->SetModified(_tx_num, lsn);
}

int Transaction::Size(const std::string &filename) {
  BlockId dummyBlock = BlockId(filename, _end_of_file);
  _concurrency_manager->SLock(dummyBlock);
  return _file_manager->Length(filename);
}

BlockId Transaction::Append(const std::string &filename) {
  BlockId dummyBlock = BlockId(filename, _end_of_file);
  _concurrency_manager->XLock(dummyBlock);
  return _file_manager->Append(filename);
}

int Transaction::BlockSize() { return _file_manager->BlockSize(); }

int Transaction::AvailableBuffs() { return _buffer_manager->Available(); }

int Transaction::nextTxNum() {
  std::unique_lock<std::mutex> lock(_mutex);
  _next_tx_num++;
  std::cout << "new transaction: " << _next_tx_num << std::endl;
  return _next_tx_num;
}

} // namespace simpledb
