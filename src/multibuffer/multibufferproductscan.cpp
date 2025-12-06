#include "multibuffer/multibufferproductscan.hpp"
#include "multibuffer/bufferneeds.hpp"
#include "multibuffer/chunkscan.hpp"
#include "query/productscan.hpp"
#include <memory>
namespace simpledb {
MultiBufferProductScan::MultiBufferProductScan(
    Transaction *tx, const std::shared_ptr<Scan> &leftHandSideScan,
    const std::string &tableName, const Layout &layout)
    : _tx(tx), _left_hand_scan(leftHandSideScan), _filename(tableName + ".tbl"),
      _layout(layout) {
  _file_size = _tx->Size(_filename);
  int available = _tx->AvailableBuffs();
  _chunk_size = BufferNeeds::BestFactor(available, _file_size);
  _next_block_num = 0;
  useNextChunk();
}

void MultiBufferProductScan::BeforeFirst() {
  _next_block_num = 0;
  useNextChunk();
}

bool MultiBufferProductScan::Next() {
  while (_product_scan->Next()) {
    if (!useNextChunk()) {
      return false;
    }
  }
  return true;
}

void MultiBufferProductScan::Close() { _product_scan->Close(); }

Constant MultiBufferProductScan::GetVal(const std::string &fieldName) {
  return _product_scan->GetVal(fieldName);
}

int MultiBufferProductScan::GetInt(const std::string &fieldName) {
  return _product_scan->GetInt(fieldName);
}

std::string MultiBufferProductScan::GetString(const std::string &fieldName) {
  return _product_scan->GetString(fieldName);
}

bool MultiBufferProductScan::HasField(const std::string &fieldName) {
  return _product_scan->HasField(fieldName);
}

bool MultiBufferProductScan::useNextChunk() {
  if (_next_block_num >= _file_size) {
    return false;
  }
  if (_right_hand_scan) {
    _right_hand_scan->Close();
  }
  int end = _next_block_num + _chunk_size - 1;
  if (end >= _file_size) {
    end = _file_size - 1;
  }
  _right_hand_scan = std::static_pointer_cast<Scan>(std::make_shared<ChunkScan>(
      _tx, _filename, _layout, _next_block_num, end));
  _left_hand_scan->BeforeFirst();
  _product_scan = std::static_pointer_cast<Scan>(
      std::make_shared<ProductScan>(_left_hand_scan, _right_hand_scan));
  _next_block_num = end + 1;
  return true;
}

} // namespace simpledb
