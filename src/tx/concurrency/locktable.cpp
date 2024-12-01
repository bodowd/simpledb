#include "tx/concurrency/locktable.hpp"
#include <chrono>
#include <mutex>
#include <stdexcept>

namespace simpledb {
LockTable::LockTable() {}

void LockTable::SLock(const BlockId &blkId) {
  std::unique_lock<std::mutex> lock(_mutex);
  auto start = std::chrono::high_resolution_clock::now();
  while (_hasXLock(blkId) && !_waitingTooLong(start)) {
    _cond_var.wait_for(lock, std::chrono::milliseconds(MAX_TIME));
  }
  if (_hasXLock(blkId)) {
    throw std::runtime_error(
        "LockTable: there is an Xlock. lock abort exception");
  }
  int val = _getLockVal(blkId);
  _locks[blkId] = val + 1;
}

void LockTable::XLock(const BlockId &blkId) {
  std::unique_lock<std::mutex> lock(_mutex);
  auto start = std::chrono::high_resolution_clock::now();
  while (_hasOtherSLocks(blkId) && !_waitingTooLong(start)) {
    _cond_var.wait_for(lock, std::chrono::milliseconds(MAX_TIME));
  }
  if (_hasOtherSLocks(blkId)) {
    throw std::runtime_error("LockTable: cannot acquire XLock because the "
                             "block has SLocks. Lock abort exception");
  }
  // a value of -1 indicates there is an exclusive lock
  _locks[blkId] = -1;
}

void LockTable::Unlock(const BlockId &blkId) {
  std::unique_lock<std::mutex> lock(_mutex);
  int val = _getLockVal(blkId);
  if (val > 1) {
    _locks[blkId]--;
  } else {
    _locks.erase(blkId);
    _cond_var.notify_all();
  }
}

bool LockTable::_hasXLock(const BlockId &blkId) {
  return _getLockVal(blkId) < 0;
}

bool LockTable::_hasOtherSLocks(const BlockId &blkId) {
  return _getLockVal(blkId) > 1;
}

bool LockTable::_waitingTooLong(
    std::chrono::time_point<std::chrono::high_resolution_clock> starttime) {
  auto end = std::chrono::high_resolution_clock::now();
  double elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - starttime)
          .count();
  return elapsed > MAX_TIME;
}

int LockTable::_getLockVal(const BlockId &blkId) {
  auto iter = _locks.find(blkId);
  if (iter == _locks.end()) {
    return 0;
  }
  return iter->second;
}
} // namespace simpledb
