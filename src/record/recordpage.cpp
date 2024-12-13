#include "record/recordpage.hpp"
namespace simpledb {
RecordPage::RecordPage(Transaction *tx, const BlockId &blk,
                       const Layout &layout) {
  _tx = tx;
  _blk = blk;
  _layout = layout;
  _tx->Pin(blk);
}

int RecordPage::GetInt(int slot, const std::string &fieldname) {
  int fldpos = offset(slot) + _layout.Offset(fieldname);
  return _tx->GetInt(_blk, fldpos);
}

std::string RecordPage::GetString(int slot, const std::string &fieldname) {
  int fldpos = offset(slot) + _layout.Offset(fieldname);
  return _tx->GetString(_blk, fldpos);
}

void RecordPage::SetInt(int slot, const std::string &fieldname, int val) {
  int fldpos = offset(slot) + _layout.Offset(fieldname);
  _tx->SetInt(_blk, fldpos, val, true);
}

void RecordPage::SetString(int slot, const std::string &fieldname,
                           const std::string &val) {
  int fldpos = offset(slot) + _layout.Offset(fieldname);
  _tx->SetString(_blk, fldpos, val, true);
}

void RecordPage::Delete(int slot) { setFlag(slot, EMPTY); }

void RecordPage::Format() {
  int slot = 0;
  while (isValidSlot(slot)) {
    _tx->SetInt(_blk, offset(slot), EMPTY, false);
    Schema sch = _layout.GetSchema();
    for (const std::string &fieldname : sch.Fields()) {
      int fldpos = offset(slot) + _layout.Offset(fieldname);
      if (sch.Type(fieldname) == Schema::INTEGER) {
        _tx->SetInt(_blk, fldpos, 0, false);
      } else {
        _tx->SetString(_blk, fldpos, "", false);
      }
    }
    slot++;
  }
}

int RecordPage::NextAfter(int slot) { return searchAfter(slot, USED); }

int RecordPage::InsertAfter(int slot) {
  int newSlot = searchAfter(slot, EMPTY);
  if (newSlot >= 0) {
    setFlag(newSlot, USED);
  }
  return newSlot;
}

BlockId RecordPage::Block() const { return _blk; }

void RecordPage::setFlag(int slot, int flag) {
  _tx->SetInt(_blk, offset(slot), flag, true);
}

int RecordPage::searchAfter(int slot, int flag) {
  slot++;
  while (isValidSlot(slot)) {
    // Transaction::GetInt will read the offset of that block for us.
    // It handles all the details of ACID for us
    if (_tx->GetInt(_blk, offset(slot)) == flag) {
      return slot;
    }
    slot++;
  }
  return -1;
}

bool RecordPage::isValidSlot(int slot) {
  return offset(slot + 1) <= _tx->BlockSize();
}

int RecordPage::offset(int slot) const {
  auto offset = slot * _layout.SlotSize();
  if (offset < 0) {
    throw std::runtime_error("negative offset");
  }
  return offset;
}
} // namespace simpledb
