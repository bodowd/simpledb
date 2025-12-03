#include "materialize/groupvalue.hpp"

namespace simpledb {

bool operator==(const GroupValue &right, const GroupValue &left) {
  for (const auto &[fieldName, valueRight] : right._vals) {
    if (left._vals.find(fieldName) == left._vals.end() ||
        left._vals.at(fieldName) != valueRight) {
      return false;
    }
  }
  return true;
}

bool operator!=(const GroupValue &right, const GroupValue &left) {
  if (right == left) {
    return false;
  } else {
    return true;
  }
}

GroupValue::GroupValue(Scan *scan, const std::vector<std::string> &fields) {
  for (const std::string &fieldName : fields) {
    _vals[fieldName] = scan->GetVal(fieldName);
  }
}

Constant GroupValue::GetVal(const std::string &fieldName) {
  return _vals.at(fieldName);
}

int GroupValue::HashCode() {
  int hashVal = 0;
  for (const auto &[fieldName, value] : _vals) {
    hashVal += value.HashCode();
  }
  return hashVal;
}
} // namespace simpledb
