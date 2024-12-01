#pragma once
#include "file/blockid.hpp"
#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>

namespace simpledb {

// p. 141 in the book
class LockTable {
public:
  LockTable();
  void SLock(const BlockId &blkId);
  void XLock(const BlockId &blkId);
  void Unlock(const BlockId &blkId);

private:
  const int MAX_TIME = 10000; // 10 seconds
  std::mutex _mutex;

  // If the value of an entry is -1, that means an exclusive lock is assigned.
  // A positive value denotes the current number of _shared_ locks assigned
  std::map<BlockId, int> _locks;
  std::condition_variable _cond_var;
  bool _hasXLock(const BlockId &blkId);
  bool _hasOtherSLocks(const BlockId &blkId);
  bool _waitingTooLong(
      std::chrono::time_point<std::chrono::high_resolution_clock> starttime);
  int _getLockVal(const BlockId &blkId);
};
} // namespace simpledb
