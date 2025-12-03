#include "materialize/recordcomparator.hpp"
namespace simpledb {

RecordComparator::RecordComparator(const std::vector<std::string> &fields)
    : _fields(fields) {};

int RecordComparator::Compare(Scan *s1, Scan *s2) {
  for (const std::string &fieldname : _fields) {
    Constant val1 = s1->GetVal(fieldname);
    Constant val2 = s2->GetVal(fieldname);
    if (val1 < val2) {
      return -1;
    }
    if (val2 < val1) {
      return 1;
    }
  }
  return 0;
};

} // namespace simpledb
