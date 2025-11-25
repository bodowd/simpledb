#include "index/btree/btreeleaf.hpp"
#include "file/blockid.hpp"
#include "index/btree/direntry.hpp"
#include <memory>
#include <type_traits>

namespace simpledb {

BTreeLeaf::BTreeLeaf(Transaction *tx, const BlockId &blk, const Layout &layout,
                     const Constant &searchKey)
    : _tx(tx), _layout(layout), _search_key(searchKey) {
  _contents = std::make_unique<BTPage>(_tx, blk, _layout);
  /// go to the position immediately before the first record containing the
  /// search key
  _current_slot = _contents->FindSlotBefore(_search_key);
  _filename = blk.Filename();
}

void BTreeLeaf::Close() { return _contents->Close(); }

bool BTreeLeaf::Next() {
  _current_slot++;
  if (_current_slot >= _contents->GetNumRecs()) {
    return TryOverflow();
  } else if (_contents->GetDataVal(_current_slot) == _search_key) {
    return true;
  } else {
    return TryOverflow();
  }
}

RID BTreeLeaf::GetDataRID() { return _contents->GetDataRid(_current_slot); }

void BTreeLeaf::Remove(const RID &dataRid) {
  while (Next()) {
    if (GetDataRID() == dataRid) {
      _contents->Remove(_current_slot);
      return;
    }
  }
}

DirEntry BTreeLeaf::Insert(const RID &dataRid) {
  /// move to the first record greater than the search key and insert the new
  /// record in that spot. If the page already contains records having the same
  /// search key, the new record is inserted at the front of the list
  if (_contents->GetFlag() >= 0 && _contents->GetDataVal(0) > _search_key) {
    Constant firstVal = _contents->GetDataVal(0);
    BlockId newBlk = _contents->Split(0, _contents->GetFlag());
    _current_slot = 0;
    _contents->SetFlag(-1);
    _contents->InsertLeaf(_current_slot, _search_key, dataRid);
    return DirEntry(firstVal, newBlk.Number());
  }

  _current_slot++;
  _contents->InsertLeaf(_current_slot, _search_key, dataRid);
  /// if the page is not full, we are done
  if (!_contents->IsFull()) {
    return DirEntry();
  }
  /// otherwise the page is full. Split the page
  Constant firstKey = _contents->GetDataVal(0);
  Constant lastKey = _contents->GetDataVal(_contents->GetNumRecs() - 1);
  /// the page is full of the same datavalues
  if (lastKey == firstKey) {
    /// create an overflow block to hold all but the first record
    BlockId newBlk = _contents->Split(1, _contents->GetFlag());
    _contents->SetFlag(newBlk.Number());
    return DirEntry();
  } else {
    /// otherwise, there are other datavalues in the page. Find where this key
    /// ends
    int splitPos = _contents->GetNumRecs() / 2;
    Constant splitKey = _contents->GetDataVal(splitPos);
    if (splitKey == firstKey) {
      /// move right, look for the next key
      while (_contents->GetDataVal(splitPos - 1) == splitKey) {
        splitPos++;
      }
      splitKey = _contents->GetDataVal(splitPos);
    } else {
      /// move left, look for the first entry that has the key
      while (_contents->GetDataVal(splitPos - 1) == splitKey) {
        splitPos--;
      }
    }
    BlockId newBlk = _contents->Split(splitPos, -1);
    return DirEntry(splitKey, newBlk.Number());
  }
};

bool BTreeLeaf::TryOverflow() {
  Constant firstKey = _contents->GetDataVal(0);
  int flag = _contents->GetFlag();
  /// the overflow contains other values, not the one we are searching for
  if (_search_key != firstKey || flag < 0) {
    return false;
  }
  _contents->Close();
  BlockId nextBlk(_filename, flag);
  _contents = std::make_unique<BTPage>(_tx, nextBlk, _layout);
  _current_slot = 0;
  return true;
}

} // namespace simpledb
