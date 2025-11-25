#pragma once
#include "index/btree/btpage.hpp"
#include "index/btree/direntry.hpp"
#include "tx/transaction.hpp"

namespace simpledb {

/// p. 341 in the book.
/// Implements the directory blocks of the BTree. The concept of the B-Tree
/// directory is on p. 329
class BTreeDir {
public:
  BTreeDir(Transaction *tx, const BlockId &blk, const Layout &layout);
  void Close();
  /// Start at the root and move down the tree until the level-0 directory block
  /// associated with the search key is found. Returns the block number of the
  /// leaf containing the search key
  int Search(const Constant &searchKey);
  void MakeNewRoot(const DirEntry &dirEntry);
  DirEntry Insert(const DirEntry &dirEntry);

private:
  Transaction *_tx;
  Layout _layout;
  std::unique_ptr<BTPage> _contents;
  std::string _filename;

  DirEntry insertEntry(const DirEntry &dirEntry);
  BlockId findChildBlock(const Constant &searchKey);
};
} // namespace simpledb
