#pragma once
#include "file/blockid.hpp"
#include "file/filemanager.hpp"
#include "log/logmanager.hpp"
#include <chrono>
#include <condition_variable>

namespace simpledb {
class Buffer {
public:
  Buffer(FileManager *fm, LogManager *lm);
  Page *Contents();
  BlockId Block();
  // Mark the Buffer with the transaction and lsn that modified the page
  // that the buffer holds
  void SetModified(int txnum, int lsn);
  bool IsPinned();
  int ModifyingTx();
  // Associate the Buffer with a disk block. Flush any modifications to previous
  // block that was stored in the Buffer, read the contents of the block to the
  // contents of the Buffer, and reset pins to zero. Client is responsible
  // for pinning and unpinning the Buffer
  void AssignToBlock(BlockId b);
  void Flush();
  void Pin();
  void Unpin();

private:
  FileManager *_file_manager;
  LogManager *_log_manager;
  std::unique_ptr<Page> _contents;
  BlockId _block_id;
  int _pins = 0;
  // this is the number/id of the transaction that last used this buffer
  int _txnum = -1;
  // if page was modified, this is the LSN of the most recent log record for
  // that modification. If the client calls SetModified with a negative lsn,
  // it indicates that a log record was not generated for that modification
  int _lsn = -1;
};

class BufferManager {
public:
  BufferManager(FileManager *fm, LogManager *lm, int num_buffs);
  int Available();
  void FlushAll(int txnum);
  void Unpin(Buffer *buff);
  Buffer *Pin(const BlockId &blk);
  // It is useful to shorten the max time for the tests
  void SetMaxTime(int max_time);

private:
  std::vector<std::unique_ptr<Buffer>> _buffer_pool;
  // Number of free/unpinned buffers
  int _num_available;
  int _max_time = 10000; // 10 seconds
  std::mutex _mutex;
  // synchronization primitive for thread communication
  // It allows threads to block or sleep until a condition is met
  std::condition_variable _condition_var;
  Buffer *tryToPin(const BlockId blk);
  // Look to see if any buffer already has this block's data in its page
  Buffer *findExistingBuffer(const BlockId blk);
  bool waitingTooLong(
      std::chrono::time_point<std::chrono::high_resolution_clock> starttime);
  Buffer *chooseUnpinnedBuffer();
};
} // namespace simpledb
