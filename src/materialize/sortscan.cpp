#include "materialize/sortscan.hpp"
#include "materialize/recordcomparator.hpp"
#include <memory>
#include <stdexcept>
namespace simpledb {
SortScan::SortScan(const std::vector<std::shared_ptr<TempTable>> &runs,
                   RecordComparator &comp)
    : _comp(comp), _has_more2(false) {

  if (runs.size() > 2) {
    throw std::runtime_error("Expected maximum of two runs, but got " +
                             std::to_string(runs.size()) + " runs");
  }
  if (runs.size() == 0) {
    throw std::runtime_error("Expected at least one run, but got none");
  }

  _scan1 = std::static_pointer_cast<UpdateScan>(runs[0]->Open());
  _has_more1 = _scan1->Next();
  if (runs.size() > 1) {
    _scan2 = std::static_pointer_cast<UpdateScan>(runs[1]->Open());
    _has_more2 = _scan2->Next();
  }
}

void SortScan::BeforeFirst() {
  _scan1->BeforeFirst();
  _has_more1 = _scan1->Next();
  if (_scan2) {
    _scan2->BeforeFirst();
  }
}

bool SortScan::Next() {
  if (_current_scan == _scan1) {
    _has_more1 = _scan1->Next();
  } else if (_current_scan == _scan2) {
    _has_more2 = _scan2->Next();
  }

  if (!_has_more1 && !_has_more2) {
    return false;
  } else if (_has_more1 && _has_more2) {
    if (_comp.Compare(_scan1.get(), _scan2.get()) < 0) {
      _current_scan = _scan1;
    } else {
      _current_scan = _scan2;
    }
  } else if (_has_more1) {
    _current_scan = _scan1;
  } else if (_has_more2) {
    _current_scan = _scan2;
  }
  return true;
}

void SortScan::Close() {
  _scan1->Close();
  if (_scan2) {
    _scan2->Close();
  }
}

Constant SortScan::GetVal(const std::string &fieldName) {
  return _current_scan->GetVal(fieldName);
}

int SortScan::GetInt(const std::string &fieldName) {
  return _current_scan->GetInt(fieldName);
}

std::string SortScan::GetString(const std::string &fieldName) {
  return _current_scan->GetString(fieldName);
}

bool SortScan::HasField(const std::string &fieldName) {
  return _current_scan->HasField(fieldName);
}

void SortScan::SavePosition() {
  RID r1 = _scan1->GetRid();
  RID r2 = _scan2->GetRid();
  _saved_position = std::vector<RID>{r1, r2};
}

void SortScan::RestorePosition() {
  RID r1 = _saved_position[0];
  RID r2 = _saved_position[1];
  _scan1->MoveToRid(r1);
  _scan2->MoveToRid(r2);
}

} // namespace simpledb
