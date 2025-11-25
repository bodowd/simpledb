#pragma once
#include "file/blockid.hpp"
#include "query/constant.hpp"
#include "record/layout.hpp"
#include "record/rid.hpp"
#include "tx/transaction.hpp"
namespace simpledb {

/// p 337 in the book
/// BTPage contains the common code for directory and leaf blocks in the B-Tree
/// For example, both directory and leaf blocks need to be able to insert
/// entries in sorted order and to split themselves
///
/// Records in a B-tree page have the following requirements:
///     - The records need to be maintained in sorted order (as opposed to a
///     record page which holds an unsorted array of records)
///     - The records do not need a permanent id so that they can be moved
///     around within the page as needed.
///     - A page needs to be able to split its records with another page
///     - Each page needs an integer to serve as a flag. The flag is used by
///     directory and leaf pages to hold different metadata
class BTPage {
public:
  BTPage(Transaction *tx, const BlockId &currentBlk, const Layout &layout);
  int FindSlotBefore(const Constant &searchKey);
  void Close();
  bool IsFull();
  BlockId Split(int splitPosition, int flag);
  Constant GetDataVal(int slot);
  /// The flag indicates the level of the block in the B-Tree, as in level 0
  /// block. -1 indicates it is a leaf block
  int GetFlag();
  void SetFlag(int val);
  BlockId AppendNew(int flag);
  /// Format the block, reset the records, setting it to a clean starting point
  void Format(const BlockId &block, int flag);

  int GetChildNum(int slot);
  void InsertDir(int slot, const Constant &val, int blockNum);

  RID GetDataRid(int slot);
  void InsertLeaf(int slot, const Constant &val, const RID &rid);
  /// When a record is deleted, the records following it are shifted to the left
  /// to fill in the hole
  void Remove(int slot);
  /// Get the number of records in the block
  int GetNumRecs();

private:
  Transaction *_tx;
  BlockId _current_block;
  Layout _layout;

  /// Reset the record to default values
  void makeDefaultRecord(const BlockId &block, int position);
  int getInt(int slot, const std::string &fieldname);
  std::string getString(int slot, const std::string &fieldname);
  Constant getVal(int slot, const std::string &fieldname);
  void setInt(int slot, const std::string &fieldname, int val);
  void setString(int slot, const std::string &fieldname,
                 const std::string &val);
  void setVal(int slot, const std::string &fieldname, const Constant &val);
  void setNumRecs(int num);
  /// When a new record is inserted into the page, its position in the sort
  /// order is determined, and the records following it are shifted one place to
  /// the right to make room
  void insert(int slot);
  void copyRecord(int from, int to);
  void transferRecords(int slot, BTPage *dest);
  /// get the position of the field in the page?
  int fieldPosition(int slot, const std::string &fieldname);
  /// get the position of the slot in the page?
  int slotPosition(int slot);
};
} // namespace simpledb
