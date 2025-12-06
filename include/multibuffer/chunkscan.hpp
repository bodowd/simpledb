#pragma once

#include "query/scan.hpp"
#include "record/layout.hpp"
#include "record/recordpage.hpp"
#include "tx/transaction.hpp"
#include <memory>
#include <vector>
namespace simpledb {

/// p. 405 in the book
/// A chunk is a k-block portion of a materialized table having the property
/// that all blocks of the chunk fit into the available buffers
class ChunkScan : public Scan {
public:
  /// startBlockNum: the first block of the chunk
  /// endBlockNum: the last block of the chunk
  ChunkScan(Transaction *tx, const std::string &fileName, const Layout &layout,
            int startBlockNum, int endBlockNum);
  void BeforeFirst() override;
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  /// Unpin all the record pages in the chunk
  void Close() override;

private:
  /// a vector of record pages for each block in the chunk
  std::vector<std::unique_ptr<RecordPage>> _buffers;
  Transaction *_tx;
  std::string _filename;
  Layout _layout;
  int _start_block_num, _end_block_num, _current_block_num;
  /// Track which record page in the chunk is being scanned
  RecordPage *_record_page;
  int _current_slot;

  void moveToBlock(int blockNum);
};
} // namespace simpledb
