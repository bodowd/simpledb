#include "materialize/mergejoinscan.hpp"
namespace simpledb {

MergeJoinScan::MergeJoinScan(const std::shared_ptr<Scan> &scan1,
                             const std::shared_ptr<SortScan> &scan2,
                             const std::string &fieldName1,
                             const std::string &fieldName2)
    : _scan1(scan1), _scan2(scan2), _field_name1(fieldName1),
      _field_name2(fieldName2) {
  _scan1->BeforeFirst();
  _scan2->BeforeFirst();
}

void MergeJoinScan::Close() {
  _scan1->Close();
  _scan2->Close();
}

void MergeJoinScan::BeforeFirst() {
  _scan1->BeforeFirst();
  _scan2->BeforeFirst();
}

bool MergeJoinScan::Next() {
  bool hasMore2 = _scan2->Next();
  if (hasMore2 && _scan2->GetVal(_field_name2) == _join_val) {
    return true;
  }
  bool hasMore1 = _scan1->Next();
  if (hasMore1 && _scan1->GetVal(_field_name1) == _join_val) {
    _scan2->RestorePosition();
    return true;
  }

  while (hasMore1 && hasMore2) {
    Constant v1 = _scan1->GetVal(_field_name1);
    Constant v2 = _scan2->GetVal(_field_name2);
    if (v1 < v2) {
      hasMore1 = _scan1->Next();
    } else if (v2 < v1) {
      hasMore2 = _scan2->Next();
    } else {
      _scan2->SavePosition();
      _join_val = _scan2->GetVal(_field_name2);
      return true;
    }
  }
  return false;
}

int MergeJoinScan::GetInt(const std::string &fieldName) {
  if (_scan1->HasField(fieldName)) {
    return _scan1->GetInt(fieldName);
  } else {
    return _scan2->GetInt(fieldName);
  }
}

std::string MergeJoinScan::GetString(const std::string &fieldName) {
  if (_scan1->HasField(fieldName)) {
    return _scan1->GetString(fieldName);
  } else {
    return _scan2->GetString(fieldName);
  }
}

Constant MergeJoinScan::GetVal(const std::string &fieldName) {
  if (_scan1->HasField(fieldName)) {
    return _scan1->GetVal(fieldName);
  } else {
    return _scan2->GetVal(fieldName);
  }
}

bool MergeJoinScan::HasField(const std::string &fieldName) {
  return _scan1->HasField(fieldName) || _scan2->HasField(fieldName);
}
} // namespace simpledb
