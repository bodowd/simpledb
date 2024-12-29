#include "record/tablescan.hpp"
#include "file/blockid.hpp"
#include "record/schema.hpp"
#include <memory>
namespace simpledb {

TableScan::TableScan(Transaction *tx, const std::string &table_name,
                     const Layout &layout) {
  _tx = tx;
  _layout = layout;
  _filename = table_name + ".tbl";
  if (_tx->Size(_filename) == 0) {
    moveToNewBlock();
  } else {
    moveToBlock(0);
  }
}

TableScan::~TableScan() {}

void TableScan::Close() {
  if (_rp) {
    _tx->Unpin(_rp->Block());
  }
}

void TableScan::BeforeFirst() { moveToBlock(0); }

bool TableScan::Next() {
  _currentSlot = _rp->NextAfter(_currentSlot);
  while (_currentSlot < 0) {
    if (atLastBlock()) {
      return false;
    }
    moveToBlock(_rp->Block().Number() + 1);
    _currentSlot = _rp->NextAfter(_currentSlot);
  }
  return true;
}

int TableScan::GetInt(const std::string &fieldname) {
  return _rp->GetInt(_currentSlot, fieldname);
}

std::string TableScan::GetString(const std::string &fieldname) {
  return _rp->GetString(_currentSlot, fieldname);
}

Constant TableScan::GetVal(const std::string &fieldname) {
  if (_layout.GetSchema().Type(fieldname) == Schema::INTEGER) {
    return Constant(GetInt(fieldname));
  } else {
    return Constant(GetString(fieldname));
  }
}

bool TableScan::HasField(const std::string &fieldname) {
  return _layout.GetSchema().HasField(fieldname);
}

void TableScan::SetInt(const std::string &fieldname, int val) {
  return _rp->SetInt(_currentSlot, fieldname, val);
}

void TableScan::SetString(const std::string &fieldname,
                          const std::string &val) {
  _rp->SetString(_currentSlot, fieldname, val);
}

void TableScan::SetVal(const std::string &fieldname, const Constant &val) {
  if (_layout.GetSchema().Type(fieldname) == Schema::INTEGER) {
    SetInt(fieldname, val.AsInt());
  } else {
    SetString(fieldname, val.AsString());
  }
}

void TableScan::Insert() {
  _currentSlot = _rp->InsertAfter(_currentSlot);
  while (_currentSlot < 0) {
    if (atLastBlock()) {
      moveToNewBlock();
    } else {
      moveToBlock(_rp->Block().Number() + 1);
    }
    _currentSlot = _rp->InsertAfter(_currentSlot);
  }
}

void TableScan::Delete() { _rp->Delete(_currentSlot); }

void TableScan::MoveToRid(const RID &rid) {
  Close();
  BlockId blk = BlockId(_filename, rid.BlockNumber());
  _rp = std::make_unique<RecordPage>(_tx, blk, _layout);
  _currentSlot = rid.Slot();
}

RID TableScan::GetRid() { return RID(_rp->Block().Number(), _currentSlot); }

void TableScan::moveToBlock(int blknum) {
  Close();
  BlockId blk(_filename, blknum);
  _rp = std::make_unique<RecordPage>(_tx, blk, _layout);
  _currentSlot = -1;
}

void TableScan::moveToNewBlock() {
  Close();
  BlockId blk = _tx->Append(_filename);
  _rp = std::make_unique<RecordPage>(_tx, blk, _layout);
  _rp->Format();
  _currentSlot = -1;
}

bool TableScan::atLastBlock() {
  return _rp->Block().Number() == _tx->Size(_filename) - 1;
}
} // namespace simpledb
