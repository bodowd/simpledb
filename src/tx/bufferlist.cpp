#include "tx/bufferlist.hpp"
#include <algorithm>

namespace simpledb {

BufferList::BufferList(BufferManager *bm) { _buffer_manager = bm; }

Buffer *BufferList::GetBuffer(const BlockId &blkId) {
  return _buffers.at(blkId);
}

void BufferList::Pin(const BlockId &blkId) {
  Buffer *buff = _buffer_manager->Pin(blkId);
  _buffers[blkId] = buff;
  _pins.emplace_back(blkId);
}

void BufferList::Unpin(const BlockId &blkId) {
  Buffer *buff = _buffers.at(blkId);
  _buffer_manager->Unpin(buff);

  auto iter = std::find(_pins.begin(), _pins.end(), blkId);
  if (iter != _pins.end()) {
    _pins.erase(iter);
  }

  iter = std::find(_pins.begin(), _pins.end(), blkId);
  // if the blkId is not in the _pins vector, it is no longer pinned
  // and it can be removed from the map because no buffer will contain this
  // blockId anymore
  if (iter == _pins.end()) {
    _buffers.erase(blkId);
  }
}

void BufferList::UnpinAll() {
  for (auto &&blkId : _pins) {
    Buffer *buff = _buffers.at(blkId);
    _buffer_manager->Unpin(buff);
  }
  _buffers.clear();
  _pins.clear();
}
} // namespace simpledb
