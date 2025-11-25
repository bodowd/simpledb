#include "index/btree/btreedir.hpp"
#include "file/blockid.hpp"
#include "index/btree/direntry.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

BTreeDir::BTreeDir(Transaction *tx, const BlockId &blk, const Layout &layout)
    : _tx(tx), _layout(layout) {
  _contents = std::make_unique<BTPage>(_tx, blk, _layout);
  _filename = blk.Filename();
}

void BTreeDir::Close() { _contents->Close(); }

int BTreeDir::Search(const Constant &searchKey) {
  BlockId childBlk = findChildBlock(searchKey);
  while (_contents->GetFlag() > 0) {
    _contents->Close();
    _contents = std::make_unique<BTPage>(_tx, childBlk, _layout);
    childBlk = findChildBlock(searchKey);
  }
  return childBlk.Number();
}

void BTreeDir::MakeNewRoot(const DirEntry &dirEntry) {
  Constant firstVal = _contents->GetDataVal(0);
  int level = _contents->GetFlag();
  BlockId newBlk = _contents->Split(0, level);
  DirEntry oldRoot = DirEntry(firstVal, newBlk.Number());
  insertEntry(oldRoot);
  insertEntry(dirEntry);
  _contents->SetFlag(level + 1);
}

DirEntry BTreeDir::Insert(const DirEntry &dirEntry) {
  /// we are at level 0
  if (_contents->GetFlag() == 0) {
    return insertEntry(dirEntry);
  }

  BlockId childBlk = findChildBlock(dirEntry.DataVal());
  BTreeDir child(_tx, childBlk, _layout);
  DirEntry entry = child.Insert(dirEntry);
  child.Close();
  if (entry.IsNull()) {
    return DirEntry();
  } else {
    return insertEntry(entry);
  }
}

DirEntry BTreeDir::insertEntry(const DirEntry &dirEntry) {
  int newSlot = 1 + _contents->FindSlotBefore(dirEntry.DataVal());
  _contents->InsertDir(newSlot, dirEntry.DataVal(), dirEntry.BlockNumber());
  if (!_contents->IsFull()) {
    return DirEntry();
  }
  /// otherwise the page is full and we need to split the page
  int level = _contents->GetFlag();
  int splitPos = _contents->GetNumRecs() / 2;
  Constant splitVal = _contents->GetDataVal(splitPos);
  BlockId newBlk = _contents->Split(splitPos, level);
  return DirEntry(splitVal, newBlk.Number());
}

BlockId BTreeDir::findChildBlock(const Constant &searchKey) {
  int slot = _contents->FindSlotBefore(searchKey);
  if (_contents->GetDataVal(slot + 1) == searchKey) {
    slot++;
  }
  int blkNum = _contents->GetChildNum(slot);
  return BlockId(_filename, blkNum);
}
} // namespace simpledb
