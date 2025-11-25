#include "index/btree/btpage.hpp"
#include "file/blockid.hpp"
#include "query/constant.hpp"
#include "record/schema.hpp"

namespace simpledb {
BTPage::BTPage(Transaction *tx, const BlockId &currentBlk, const Layout &layout)
    : _tx(tx), _current_block(currentBlk), _layout(layout) {
  _tx->Pin(_current_block);
}

int BTPage::FindSlotBefore(const Constant &searchKey) {
  int slot = 0;
  while (slot < GetNumRecs() && GetDataVal(slot) < searchKey) {
    slot++;
  }
  return slot - 1;
}

void BTPage::Close() {
  if (!_current_block.IsNull()) {
    _tx->Unpin(_current_block);
  }
  /// reset the current block
  _current_block = BlockId();
}

bool BTPage::IsFull() {
  return slotPosition(GetNumRecs() + 1) >= _tx->BlockSize();
}

BlockId BTPage::Split(int splitPosition, int flag) {
  BlockId newBlock = AppendNew(flag);
  BTPage newPage(_tx, newBlock, _layout);
  transferRecords(splitPosition, &newPage);
  newPage.SetFlag(flag);
  newPage.Close();
  return newBlock;
}

Constant BTPage::GetDataVal(int slot) {
  /// In the record of the index, the value of the record is stored under the
  /// "dataval" field
  return getVal(slot, "dataval");
}

int BTPage::GetFlag() {
  /// We store the flag for the page in offset 0 of the current block
  return _tx->GetInt(_current_block, 0);
}

void BTPage::SetFlag(int val) {
  /// The flag for the page is stored in offset 0 of the current block
  _tx->SetInt(_current_block, 0, val, true);
}

BlockId BTPage::AppendNew(int flag) {
  BlockId blk = _tx->Append(_current_block.Filename());
  _tx->Pin(blk);
  Format(blk, flag);
  return blk;
}

void BTPage::Format(const BlockId &blk, int flag) {
  /// the first byte, starting at offset 0, is the flag for the BTPage. The Flag
  /// indicates what level of the BTree the page is in, for example a level 0
  /// block which has children that contain the index records or level 1
  /// directory block
  _tx->SetInt(blk, 0, flag, false);
  /// Set the number of records to 0. This is stored at offset sizeof(int) -- 4
  /// bytes
  _tx->SetInt(blk, sizeof(int), 0, false);
  int recordSize = _layout.SlotSize();
  for (int position = 2 * sizeof(int);
       position + recordSize <= _tx->BlockSize(); position += recordSize) {
    makeDefaultRecord(blk, position);
  };
}

void BTPage::makeDefaultRecord(const BlockId &block, int position) {
  for (const std::string &fieldname : _layout.GetSchema().Fields()) {
    int offset = _layout.Offset(fieldname);
    if (_layout.GetSchema().Type(fieldname) == Schema::INTEGER) {
      _tx->SetInt(block, position + offset, 0, false);
    } else {
      _tx->SetString(block, position + offset, "", false);
    }
  }
}

int BTPage::GetChildNum(int slot) {
  // the "block" field contains the block number
  return getInt(slot, "block");
}

void BTPage::InsertDir(int slot, const Constant &val, int blockNum) {
  insert(slot);
  setVal(slot, "dataval", val);
  setInt(slot, "block", blockNum);
}

RID BTPage::GetDataRid(int slot) {
  return RID(getInt(slot, "block"), getInt(slot, "id"));
}

void BTPage::InsertLeaf(int slot, const Constant &val, const RID &rid) {
  insert(slot);
  setVal(slot, "dataval", val);
  setInt(slot, "block", rid.BlockNumber());
  setInt(slot, "id", rid.Slot());
}

void BTPage::Remove(int slot) {
  for (int i = slot + 1; i < GetNumRecs(); i++) {
    copyRecord(i, i - 1);
  }
  setNumRecs(GetNumRecs() - 1);
  return;
}

int BTPage::GetNumRecs() { return _tx->GetInt(_current_block, sizeof(int)); }

int BTPage::getInt(int slot, const std::string &fieldname) {
  int pos = fieldPosition(slot, fieldname);
  return _tx->GetInt(_current_block, pos);
}

std::string BTPage::getString(int slot, const std::string &fieldname) {
  int pos = fieldPosition(slot, fieldname);
  return _tx->GetString(_current_block, pos);
}

Constant BTPage::getVal(int slot, const std::string &fieldname) {
  int type = _layout.GetSchema().Type(fieldname);
  if (type == Schema::INTEGER) {
    return Constant(getInt(slot, fieldname));
  } else {
    return Constant(getString(slot, fieldname));
  }
}

void BTPage::setInt(int slot, const std::string &fieldname, int val) {
  int pos = fieldPosition(slot, fieldname);
  _tx->SetInt(_current_block, pos, val, true);
}

void BTPage::setString(int slot, const std::string &fieldname,
                       const std::string &val) {
  int pos = fieldPosition(slot, fieldname);
  _tx->SetString(_current_block, pos, val, true);
}

void BTPage::setVal(int slot, const std::string &fieldname,
                    const Constant &val) {
  int type = _layout.GetSchema().Type(fieldname);
  if (type == Schema::INTEGER) {
    setInt(slot, fieldname, val.AsInt());
  } else {
    setString(slot, fieldname, val.AsString());
  }
}

void BTPage::setNumRecs(int num) {
  /// the number of records is set starting from the  first byte of the current
  /// block
  _tx->SetInt(_current_block, sizeof(int), num, true);
}

void BTPage::insert(int slot) {
  for (int i = GetNumRecs(); i > slot; i--) {
    /// shift the record one place to the right
    copyRecord(i - 1, i);
  }
  setNumRecs(GetNumRecs() + 1);
}

void BTPage::copyRecord(int from, int to) {
  Schema sch(_layout.GetSchema());
  for (const std::string &fieldname : sch.Fields()) {
    setVal(to, fieldname, getVal(from, fieldname));
  }
}

void BTPage::transferRecords(int slot, BTPage *dest) {
  int dest_slot = 0;
  while (slot < GetNumRecs()) {
    dest->insert(dest_slot);
    Schema sch(_layout.GetSchema());
    for (const std::string &fieldname : sch.Fields()) {
      dest->setVal(dest_slot, fieldname, getVal(slot, fieldname));
      Remove(slot);
      dest_slot++;
    }
  }
}

int BTPage::fieldPosition(int slot, const std::string &fieldname) {
  int offset = _layout.Offset(fieldname);
  return slotPosition(slot) + offset;
}

int BTPage::slotPosition(int slot) {
  int slotsize = _layout.SlotSize();
  return sizeof(int) + sizeof(int) + (slot * slotsize);
}

} // namespace simpledb
