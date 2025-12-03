#include "materialize/maxfn.hpp"
#include <string>

namespace simpledb {

MaxFn::MaxFn(const std::string &fieldName) : _field_name(fieldName) {}

void MaxFn::ProcessFirst(Scan *scan) { _val = scan->GetVal(_field_name); }

void MaxFn::ProcessNext(Scan *scan) {
  Constant newVal = scan->GetVal(_field_name);
  if (newVal > _val) {
    _val = newVal;
  }
}

std::string MaxFn::FieldName() { return "maxof" + _field_name; }

Constant MaxFn::Value() { return _val; }
} // namespace simpledb
