#include "index/btree/btreeindex.hpp"
#include "index/btree/btreedir.hpp"
#include "index/btree/direntry.hpp"
#include <cmath>
#include <limits>
#include <memory>
namespace simpledb {
BTreeIndex::BTreeIndex(Transaction *tx, const std::string &idxName,
                       const Layout &leafLayout)
    : _tx(tx), _leaf_table(idxName + "leaf"), _leaf_layout(leafLayout) {

  /// handle the leaves
  if (_tx->Size(_leaf_table) == 0) {
    BlockId block = _tx->Append(_leaf_table);
    BTPage node(_tx, block, _leaf_layout);
    node.Format(block, -1);
  }

  /// handle the directory
  Schema dirSchema;
  dirSchema.Add("block", _leaf_layout.GetSchema());
  dirSchema.Add("dataval", _leaf_layout.GetSchema());
  std::string dirTableFilename = idxName + "dir";
  _dir_layout = Layout(dirSchema);
  _root_block = BlockId(dirTableFilename, 0);
  if (_tx->Size(dirTableFilename) == 0) {
    /// create a new root block
    _tx->Append(dirTableFilename);
    BTPage node(_tx, _root_block, _dir_layout);
    node.Format(_root_block, 0);

    /// insert the initial directory entry
    int fieldType = dirSchema.Type("dataval");
    Constant minVal = (fieldType == Schema::INTEGER
                           ? Constant(std::numeric_limits<int>::min())
                           : Constant(""));
    node.InsertDir(0, minVal, 0);
    node.Close();
  }
}

void BTreeIndex::BeforeFirst(const Constant &searchKey) {
  Close();
  BTreeDir root(_tx, _root_block, _dir_layout);
  int blkNum = root.Search(searchKey);
  root.Close();
  BlockId leafBlock(_leaf_table, blkNum);
  _leaf = std::make_unique<BTreeLeaf>(_tx, leafBlock, _leaf_layout, searchKey);
}

bool BTreeIndex::Next() { return _leaf->Next(); }

RID BTreeIndex::GetDataRid() { return _leaf->GetDataRID(); }

void BTreeIndex::Insert(const Constant &dataVal, const RID &dataRid) {
  BeforeFirst(dataVal);
  DirEntry entry = _leaf->Insert(dataRid);
  _leaf->Close();
  if (entry.IsNull()) {
    return;
  }

  // because entry is not null, that means the insertion into the leaf node
  // caused a split, and returned a new index block
  // Now make a new DirEntry and then make that the new root
  BTreeDir root(_tx, _root_block, _dir_layout);
  DirEntry newRootEntry = root.Insert(entry);
  if (newRootEntry.IsNull()) {
    root.MakeNewRoot(newRootEntry);
  }
  root.Close();
}

void BTreeIndex::Remove(const Constant &dataVal, const RID &dataRid) {
  BeforeFirst(dataVal);
  _leaf->Remove(dataRid);
  _leaf->Close();
}

void BTreeIndex::Close() {
  if (_leaf) {
    _leaf->Close();
  }
}

int BTreeIndex::SearchCost(int numBlocks, int recordsPerBlock) {
  return 1 + std::round(std::log(numBlocks) / std::log(recordsPerBlock));
}
} // namespace simpledb
