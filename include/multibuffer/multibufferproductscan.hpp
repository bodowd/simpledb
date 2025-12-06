#pragma once

#include "query/scan.hpp"
#include "record/layout.hpp"
#include "tx/transaction.hpp"
namespace simpledb {

/// p. 409 in the book
class MultiBufferProductScan : public Scan {
public:
  MultiBufferProductScan(Transaction *tx,
                         const std::shared_ptr<Scan> &leftHandSideScan,
                         const std::string &tableName, const Layout &layout);
  void BeforeFirst() override;
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  void Close() override;

private:
  Transaction *_tx;
  std::shared_ptr<Scan> _left_hand_scan, _right_hand_scan, _product_scan;
  std::string _filename;
  Layout _layout;
  int _chunk_size, _next_block_num, _file_size;
  bool useNextChunk();
};
} // namespace simpledb
