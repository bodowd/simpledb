#include "query/selectscan.hpp"
#include <memory>
#include <stdexcept>

namespace simpledb {

SelectScan::SelectScan(std::shared_ptr<Scan> s, const Predicate &p)
    : _scan(s), _predicate(p) {}

void SelectScan::BeforeFirst() { _scan->BeforeFirst(); }

bool SelectScan::Next() {
  while (_scan->Next()) {
    if (_predicate.IsSatisfied(_scan.get())) {
      return true;
    }
  }
  return false;
}

int SelectScan::GetInt(const std::string &fieldname) {
  return _scan->GetInt(fieldname);
}

std::string SelectScan::GetString(const std::string &fieldname) {
  return _scan->GetString(fieldname);
}

Constant SelectScan::GetVal(const std::string &fieldname) {
  return _scan->GetVal(fieldname);
}

bool SelectScan::HasField(const std::string &fieldname) {
  return _scan->HasField(fieldname);
}

void SelectScan::Close() { _scan->Close(); }

void SelectScan::SetInt(const std::string &fieldname, int val) {
  auto us = castScan(_scan);
  us->SetInt(fieldname, val);
}

void SelectScan::SetString(const std::string &fieldname,
                           const std::string &val) {
  auto us = castScan(_scan);
  us->SetString(fieldname, val);
}

void SelectScan::SetVal(const std::string &fieldname, const Constant &val) {
  auto us = castScan(_scan);
  us->SetVal(fieldname, val);
}

void SelectScan::Insert() {
  auto us = castScan(_scan);
  return us->Insert();
}

void SelectScan::Delete() {
  auto us = castScan(_scan);
  us->Delete();
}

RID SelectScan::GetRid() {
  auto us = castScan(_scan);
  return us->GetRid();
}

void SelectScan::MoveToRid(const RID &rid) {
  auto us = castScan(_scan);
  return us->MoveToRid(rid);
}

std::shared_ptr<UpdateScan> SelectScan::castScan(std::shared_ptr<Scan> s) {
  std::shared_ptr<UpdateScan> us = std::dynamic_pointer_cast<UpdateScan>(_scan);
  if (us) {
    return us;
  } else {
    // was not able to cast
    throw std::runtime_error("Cannot cast to UpdateScan");
  }
}
} // namespace simpledb
