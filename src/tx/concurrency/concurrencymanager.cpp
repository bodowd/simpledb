#include "tx/concurrency/concurrencymanager.hpp"
namespace simpledb {

LockTable ConcurrencyManager::_lock_table;

void ConcurrencyManager::SLock(const BlockId &blkId) {
  if (_locks.find(blkId) == _locks.end()) {
    _lock_table.SLock(blkId);
    _locks[blkId] = "S";
  }
}

void ConcurrencyManager::XLock(const BlockId &blkId) {
  if (!_hasXLock(blkId)) {
    SLock(blkId);
    _lock_table.XLock(blkId);
    _locks[blkId] = "X";
  }
}

void ConcurrencyManager::Release() {
  for (auto &item : _locks) {
    _lock_table.Unlock(item.first);
  }
  _locks.clear();
}

bool ConcurrencyManager::_hasXLock(const BlockId &blkId) {
  auto iter = _locks.find(blkId);
  return (iter != _locks.end() && iter->second == "X");
}
} // namespace simpledb
