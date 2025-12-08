#pragma once
#include "query/predicate.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>
namespace simpledb {

/// The QueryData organizes data about the fields, tables, and predicates
/// mentioned in a query
class QueryData {
public:
  QueryData(const std::vector<std::string> fields,
            const std::set<std::string> &tables, const Predicate &pred,
            const std::vector<std::string> &aggFuncs);

  std::vector<std::string> Fields() const;
  std::set<std::string> Tables() const;
  Predicate Pred() const;
  std::string ToString() const;

private:
  /// These fields have no aggregation function related to them
  std::vector<std::string> _fields;
  std::set<std::string> _tables;
  /// These fields are to be summed
  std::vector<std::string> _sum_fields;
  Predicate _pred;
};
} // namespace simpledb
