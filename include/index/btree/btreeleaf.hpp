#pragma once
#include "index/btree/btpage.hpp"
#include "index/btree/direntry.hpp"
#include <memory>

namespace simpledb {

/// p. 337 in the book
/// The leaf blocks of the B-Tree
class BTreeLeaf {
public:
  BTreeLeaf(Transaction *tx, const BlockId &blk, const Layout &layout,
            const Constant &searchKey);
  void Close();
  /// Move to the next record in the BTree Page. Returns true or false depending
  /// on whether that record has the desired search key
  bool Next();
  RID GetDataRID();
  void Remove(const RID &dataRid);
  /// If the insertion does not cause the block to split, this returns null. If
  /// a split does occur, the return value is the entry corresponding to the new
  /// index block (data value, block number)
  DirEntry Insert(const RID &dataRid);
  /// Check if the leaf block contains an overflow chain.
  /// p. 335 in the book explains the concept
  /// An index block can always be split if its records contain at least two
  /// different data values. If all the records in the index block have the same
  /// data value, splitting is of no use. Instead, the best approach in this
  /// case is to use an overflow block
  ///
  /// Moves the current slot to the overflow block if there are more values that
  /// match the search key in the overflow
  bool TryOverflow();

private:
  Transaction *_tx;
  Layout _layout;
  Constant _search_key;
  std::unique_ptr<BTPage> _contents;
  int _current_slot;
  std::string _filename;
};
} // namespace simpledb
