#pragma once

#include "index/index.hpp"
#include "record/layout.hpp"
#include "record/tablescan.hpp"
#include "tx/transaction.hpp"
#include <string>

namespace simpledb {
class HashIndex : public Index {
public:
  HashIndex(Transaction *tx, const std::string &indexName,
            const Layout &layout);
  void BeforeFirst(const Constant &searchKey) override;
  bool Next() override;
  RID GetDataRid() override;
  void Insert(const Constant &dataVal, const RID &dataRID) override;
  void Remove(const Constant &dataVal, const RID &dataRID) override;
  void Close() override;
  static int NUM_BUCKETS;
  static int SearchCost(int numBlocks);

private:
  Transaction *_tx;
  std::string _indexname;
  Layout _layout;
  Constant _search_key;
  std::unique_ptr<TableScan> _table_scan;
};
} // namespace simpledb
