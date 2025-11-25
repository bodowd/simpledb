#pragma once

#include "index/btree/btreeleaf.hpp"
#include "index/index.hpp"
#include "record/layout.hpp"
#include "record/rid.hpp"
#include "tx/transaction.hpp"
namespace simpledb {
class BTreeIndex : public Index {
public:
  BTreeIndex(Transaction *tx, const std::string &idxName,
             const Layout &leafLayout);

  /// Navigate to the the position right before the first block in the leaf node
  /// entry that matches the search key. This will set the _leaf variable to
  /// this block, and a call to _leaf.Next() will return records matching the
  /// search key
  void BeforeFirst(const Constant &searchKey) override;
  bool Next() override;
  RID GetDataRid() override;
  void Insert(const Constant &dataVal, const RID &dataRid) override;
  void Remove(const Constant &dataVal, const RID &dataRid) override;
  void Close() override;
  static int SearchCost(int numBlocks, int recordsPerBlock);

private:
  Transaction *_tx;
  std::string _leaf_table;
  Layout _dir_layout, _leaf_layout;
  std::unique_ptr<BTreeLeaf> _leaf;
  BlockId _root_block;
};
} // namespace simpledb
