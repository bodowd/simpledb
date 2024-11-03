#pragma once

#include "file/blockid.hpp"
#include "file/filemanager.hpp"
#include "log/logmanager.hpp"
#include "tx/recovery/recoverymanager.hpp"
namespace simpledb {
class Transaction {
public:
  Transaction(FileManager *fm, LogManager *lm, BufferManager *bm);
  void Commit();
  void Rollback();
  void Recover();
  void Pin(const BlockId &blk);
  void Unpin(const BlockId &blk);
  int GetInt(const BlockId &blk, int offset);
  std::string GetString(const BlockId &blk, int offset);
  void SetInt(const BlockId &blk, int offset, int val, bool okToLog);
  void SetString(const BlockId &blk, int offset, std::string val, bool okToLog);
  int Size(const std::string &filename);
  BlockId Append(const std::string &filename);
  int BlockSize();
  int AvailableBuffs();
  int Num() const;

private:
  static int _next_tx_num;
  const int _end_of_file = -1;
  std::unique_ptr<RecoveryManager> _recovery_manager;
  // std::unique_ptr<ConcurrencyManager> _concurrency_manager;
  // std::unique_ptr<BufferList> _buffer_list;
  BufferManager *_buffer_manager;
  FileManager *_file_manager;
  LogManager *_log_manager;
  int _tx_num;
  static int nextTxNum();
};
} // namespace simpledb
