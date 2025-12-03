#include "materialize/groupbyscan.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
namespace simpledb {

GroupByScan::GroupByScan(
    const std::shared_ptr<Scan> &scan,
    const std::vector<std::string> &groupFields,
    const std::vector<std::shared_ptr<AggregationFn>> &aggregationFunctions)
    : _scan(scan), _group_fields(groupFields),
      _aggregation_functions(aggregationFunctions) {
  _scan->BeforeFirst();
  _more_groups = _scan->Next();
}

void GroupByScan::BeforeFirst() {
  _scan->BeforeFirst();
  _more_groups = _scan->Next();
}

bool GroupByScan::Next() {
  if (!_more_groups) {
    return false;
  }

  for (const auto &fn : _aggregation_functions) {
    fn->ProcessFirst(_scan.get());
  }

  _group_val = std::make_unique<GroupValue>(_scan.get(), _group_fields);
  while ((_more_groups = _scan->Next())) {
    auto groupVal = std::make_unique<GroupValue>(_scan.get(), _group_fields);
    if (*_group_val != *groupVal) {
      break;
    }
    for (const auto &fn : _aggregation_functions) {
      fn->ProcessNext(_scan.get());
    }
  }
  return true;
}

void GroupByScan::Close() { _scan->Close(); }

Constant GroupByScan::GetVal(const std::string &fieldName) {
  if (std::find(_group_fields.begin(), _group_fields.end(), fieldName) !=
      _group_fields.end()) {
    return _group_val->GetVal(fieldName);
  }
  for (const auto &fn : _aggregation_functions) {
    if (fn->FieldName() == fieldName) {
      return fn->Value();
    }
  }
  throw std::runtime_error("Error in GroupByScan::GetVal -- field " +
                           fieldName + " not found");
}

int GroupByScan::GetInt(const std::string &fieldName) {
  return GetVal(fieldName).AsInt();
}

std::string GroupByScan::GetString(const std::string &fieldName) {
  return GetVal(fieldName).AsString();
}

bool GroupByScan::HasField(const std::string &fieldName) {
  if (std::find(_group_fields.begin(), _group_fields.end(), fieldName) !=
      _group_fields.end()) {
    return true;
  }
  for (const auto &fn : _aggregation_functions) {
    if (fn->FieldName() == fieldName) {
      return true;
    }
  }
  return false;
}
} // namespace simpledb
