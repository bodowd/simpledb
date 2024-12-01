#pragma once
#include "buffer/buffermanager.hpp"
#include <map>
#include <memory>
#include <vector>
namespace simpledb {
// Manages the currently pinned buffers for a transaction
class BufferList {
public:
  BufferList(BufferManager *bm);
  Buffer *GetBuffer(const BlockId &blkId);
  void Pin(const BlockId &blkId);
  void Unpin(const BlockId &blkId);
  void UnpinAll();

private:
  // This maps which BlockId is assigned to which Buffer.
  std::map<BlockId, Buffer *> _buffers;
  // the vector contains a BlockId object as many times as it is pinned;
  // each time the block is unpinned, one instance is removed from the list
  std::vector<BlockId> _pins;
  BufferManager *_buffer_manager;
};
} // namespace simpledb
