#include "file/blockid.hpp"
#include "file/filemanager.hpp"
#include "log/logmanager.hpp"
#include <buffer/buffermanager.hpp>
#include <chrono>
#include <memory>
#include <mutex>
#include <stdexcept>
namespace simpledb {

Buffer::Buffer(FileManager *fm, LogManager *lm) {
  _file_manager = fm;
  _log_manager = lm;
  _contents = std::make_unique<Page>(fm->BlockSize());
}

Page *Buffer::Contents() { return _contents.get(); }

BlockId Buffer::Block() { return _block_id; }

void Buffer::SetModified(int txnum, int lsn) {
  _txnum = txnum;
  if (lsn >= 0) {
    _lsn = lsn;
  }
}

bool Buffer::IsPinned() { return _pins > 0; }

int Buffer::ModifyingTx() { return _txnum; }

void Buffer::AssignToBlock(BlockId b) {
  Flush();
  _block_id = b;
  _file_manager->Read(_block_id, *_contents);
  _pins = 0;
}

void Buffer::Flush() {
  if (_txnum >= 0) {
    _log_manager->Flush(_lsn);
    _file_manager->Write(_block_id, *_contents);
    _txnum = -1;
  }
}

void Buffer::Pin() { _pins++; }

void Buffer::Unpin() { _pins--; }

BufferManager::BufferManager(FileManager *fm, LogManager *lm, int num_buffs) {
  _num_available = num_buffs;
  for (int i = 0; i < num_buffs; i++) {
    auto buffer_ptr = std::make_unique<Buffer>(fm, lm);
    _buffer_pool.emplace_back(std::move(buffer_ptr));
  }
}

int BufferManager::Available() { return _num_available; }

void BufferManager::FlushAll(int txnum) {
  std::unique_lock<std::mutex> lock(_mutex);
  for (auto &&buff : _buffer_pool) {
    if (buff->ModifyingTx() == txnum) {
      buff->Flush();
    }
  }
}

void BufferManager::Unpin(Buffer *buff) {
  std::unique_lock<std::mutex> lock(_mutex);
  buff->Unpin();
  if (!buff->IsPinned()) {
    _num_available++;
    // wake up all threads waiting
    _condition_var.notify_all();
  }
}

Buffer *BufferManager::tryToPin(const BlockId blk) {
  Buffer *buff = findExistingBuffer(blk);
  if (!buff) {
    buff = chooseUnpinnedBuffer();
    // no free buffers
    if (!buff) {
      return nullptr;
    }
    buff->AssignToBlock(blk);
  }
  // if the buffer is unpinned, the buffer manager counts it as a
  // free buffer in num_avialable. We will need to pin this which makes it
  // unavailable. Since it will be pinned, we need to decrement the
  // num_available, which we do first here, and the next line we add a pin.
  if (!buff->IsPinned()) {
    _num_available--;
  }
  // We don't increment num_available on the pin because as long as
  // there is at least 1 pin, it is not available. more than 1 pins does
  // not change availability
  buff->Pin();
  return buff;
}

Buffer *BufferManager::findExistingBuffer(const BlockId blk) {
  for (auto &&buff : _buffer_pool) {
    BlockId b = buff->Block();
    if (!b.IsNull() && b == blk) {
      return buff.get();
    }
  }
  return nullptr;
}

Buffer *BufferManager::Pin(const BlockId &blk) {
  std::unique_lock<std::mutex> lock(_mutex);
  auto start = std::chrono::high_resolution_clock::now();
  auto buff = tryToPin(blk);
  while (!buff && !waitingTooLong(start)) {
    _condition_var.wait_for(lock, std::chrono::milliseconds(_max_time));
    buff = tryToPin(blk);
  }

  if (!buff) {
    throw std::runtime_error("BufferAbortException");
  }

  return buff;
}

void BufferManager::SetMaxTime(int max_time) { _max_time = max_time; }

bool BufferManager::waitingTooLong(
    std::chrono::time_point<std::chrono::high_resolution_clock> starttime) {
  auto end = std::chrono::high_resolution_clock::now();
  double elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - starttime)
          .count();
  return elapsed > _max_time;
}

Buffer *BufferManager::chooseUnpinnedBuffer() {
  // implements naive algorithm
  for (auto &&buff : _buffer_pool) {
    if (!buff->IsPinned()) {
      return buff.get();
    }
  }
  // no free buffers
  return nullptr;
}

} // namespace simpledb
