#pragma once
#include "file/blockid.hpp"
#include "tx/concurrency/locktable.hpp"
#include <map>

namespace simpledb {
class ConcurrencyManager {
public:
  void SLock(const BlockId &blkId);
  void XLock(const BlockId &blkId);
  void Release();

private:
  // Although each transaction has its own concurrency manager, they all share
  // the same lock table
  static LockTable _lock_table;
  // Maps each locked block to the type of lock that is on it
  // The entry can be either "S" or "X".
  // "S" means shared lock
  // "X" means exclusive lock
  std::map<BlockId, std::string> _locks;
  bool _hasXLock(const BlockId &blkId);
};
} // namespace simpledb
