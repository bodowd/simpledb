#include "parse/querydata.hpp"
#include "query/predicate.hpp"

namespace simpledb {
QueryData::QueryData(const std::vector<std::string> fields,
                     const std::set<std::string> &tables, const Predicate &pred,
                     const std::vector<std::string> &aggFuncs)
    : _fields(fields), _tables(tables), _sum_fields(aggFuncs), _pred(pred) {}

std::vector<std::string> QueryData::Fields() const { return _fields; }

std::set<std::string> QueryData::Tables() const { return _tables; }

Predicate QueryData::Pred() const { return _pred; }

std::string QueryData::ToString() const {
  std::string result = "select ";
  for (const std::string &fieldName : _fields) {
    result += fieldName + ", ";
  }
  // remove the final comma and space
  result = result.substr(0, result.size() - 2);
  result += " from ";
  for (const std::string &tableName : _tables) {
    result += tableName + ", ";
  }
  // remove the final comma and space
  result = result.substr(0, result.size() - 2);
  std::string predString = _pred.ToString();
  if (!predString.empty()) {
    result += " where " + predString;
  }
  return result;
}
} // namespace simpledb
