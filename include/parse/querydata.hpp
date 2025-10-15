#pragma once
#include "query/predicate.hpp"
#include <set>
#include <string>
#include <vector>
namespace simpledb {

/// The QueryData organizes data about the fields, tables, and predicates
/// mentioned in a query
class QueryData {
public:
  QueryData(const std::vector<std::string> fields,
            const std::set<std::string> &tables, const Predicate &pred);

  std::vector<std::string> Fields() const;
  std::set<std::string> Tables() const;
  Predicate Pred() const;
  std::string ToString() const;

private:
  std::vector<std::string> _fields;
  std::set<std::string> _tables;
  Predicate _pred;
};
} // namespace simpledb
