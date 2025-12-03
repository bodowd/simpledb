#pragma once

#include "query/constant.hpp"
#include "query/scan.hpp"
#include <map>
#include <vector>
namespace simpledb {
class GroupValue {
  friend bool operator==(const GroupValue &right, const GroupValue &left);
  friend bool operator!=(const GroupValue &right, const GroupValue &left);

public:
  GroupValue(Scan *scan, const std::vector<std::string> &fields);
  Constant GetVal(const std::string &fieldName);
  int HashCode();

private:
  std::map<std::string, Constant> _vals;
};
} // namespace simpledb
