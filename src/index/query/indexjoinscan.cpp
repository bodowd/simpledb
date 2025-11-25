#include "index/query/indexjoinscan.hpp"
#include <memory>
namespace simpledb {
IndexJoinScan::IndexJoinScan(const std::shared_ptr<Scan> &left_hand_side,
                             const std::shared_ptr<Index> &index,
                             const std::string &joinField,
                             const std::shared_ptr<TableScan> &right_hand_side)
    : _left_hand_side(left_hand_side), _index(index), _join_field(joinField),
      _right_hand_side(right_hand_side) {
  _left_hand_side->BeforeFirst();
  _left_hand_side->Next();
  ResetIndex();
}

void IndexJoinScan::BeforeFirst() {
  /// Set t1's scan to the first record
  _left_hand_side->BeforeFirst();
  /// obtain the value for this first record
  _left_hand_side->Next();
  /// set the index to right before this value
  ResetIndex();
}

bool IndexJoinScan::Next() {
  while (true) {
    if (_index->Next()) {
      _right_hand_side->MoveToRid(_index->GetDataRid());
      return true;
    }
    if (!_left_hand_side->Next()) {
      return false;
    }
    ResetIndex();
  }
}

int IndexJoinScan::GetInt(const std::string &fieldName) {
  if (_right_hand_side->HasField(fieldName)) {
    return _right_hand_side->GetInt(fieldName);
  } else {
    return _left_hand_side->GetInt(fieldName);
  }
}

std::string IndexJoinScan::GetString(const std::string &fieldName) {
  if (_right_hand_side->HasField(fieldName)) {
    return _right_hand_side->GetString(fieldName);
  } else {
    return _left_hand_side->GetString(fieldName);
  }
}

Constant IndexJoinScan::GetVal(const std::string &fieldName) {
  if (_right_hand_side->HasField(fieldName)) {
    return _right_hand_side->GetVal(fieldName);
  } else {
    return _left_hand_side->GetVal(fieldName);
  }
}

bool IndexJoinScan::HasField(const std::string &fieldName) {
  return _right_hand_side->HasField(fieldName) ||
         _left_hand_side->HasField(fieldName);
}

void IndexJoinScan::Close() {
  _left_hand_side->Close();
  _index->Close();
  _right_hand_side->Close();
}

void IndexJoinScan::ResetIndex() {
  Constant searchKey = _left_hand_side->GetVal(_join_field);
  _index->BeforeFirst(searchKey);
}
} // namespace simpledb
