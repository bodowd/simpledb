#pragma once

#include "query/scan.hpp"
#include <string>
#include <vector>
namespace simpledb {
/// p. 376 in the book
/// Compares the current records of two scans for the sort plan
///
class RecordComparator {
public:
  RecordComparator(const std::vector<std::string> &fields);
  /// Compare the current values of scan 1 and scan 2
  /// Returns -1 if the value of scan 1 is less than value from scan 2
  /// Returns 1 if the value of scan 1 is greater than value from scan 2
  /// Returns 0 if they are equal
  int Compare(Scan *s1, Scan *s2);

private:
  std::vector<std::string> _fields;
};
} // namespace simpledb
