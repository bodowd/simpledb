#include "multibuffer/chunkscan.hpp"
#include "file/blockid.hpp"
#include <memory>
namespace simpledb {

ChunkScan::ChunkScan(Transaction *tx, const std::string &fileName,
                     const Layout &layout, int startBlockNum, int endBlockNum)
    : _tx(tx), _filename(fileName), _layout(layout),
      _start_block_num(startBlockNum), _end_block_num(endBlockNum) {

  /// for each block in the chunk, open a record page (i.e. a buffer) and store
  /// them
  for (int i = _start_block_num; i <= _end_block_num; i++) {
    BlockId blk(_filename, i);
    auto recordPage = std::make_unique<RecordPage>(_tx, blk, _layout);
    _buffers.emplace_back(std::move(recordPage));
  }
  moveToBlock(_start_block_num);
}

void ChunkScan::BeforeFirst() { moveToBlock(_start_block_num); }

void ChunkScan::Close() {
  for (int i = 0; i < _buffers.size(); i++) {
    BlockId blk(_filename, _start_block_num + i);
    _tx->Unpin(blk);
  }
}

bool ChunkScan::Next() {
  _current_slot = _record_page->NextAfter(_current_slot);
  while (_current_slot < 0) {
    if (_current_block_num == _end_block_num) {
      return false;
    }
    moveToBlock(_record_page->Block().Number() + 1);
    _current_slot = _record_page->NextAfter(_current_slot);
  }
  return true;
}

int ChunkScan::GetInt(const std::string &fieldName) {
  return _record_page->GetInt(_current_slot, fieldName);
}

std::string ChunkScan::GetString(const std::string &fieldName) {
  return _record_page->GetString(_current_slot, fieldName);
}

Constant ChunkScan::GetVal(const std::string &fieldName) {
  if (_layout.GetSchema().Type(fieldName) == Schema::INTEGER) {
    return Constant(GetInt(fieldName));
  } else {
    return Constant(GetString(fieldName));
  }
}

bool ChunkScan::HasField(const std::string &fieldName) {
  return _layout.GetSchema().HasField(fieldName);
}

void ChunkScan::moveToBlock(int blockNum) {
  _current_block_num = blockNum;
  _record_page = _buffers[_current_block_num - _start_block_num].get();
  _current_slot = -1;
}
} // namespace simpledb
