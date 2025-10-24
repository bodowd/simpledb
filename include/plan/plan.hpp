#pragma once
#include "query/scan.hpp"
#include "record/schema.hpp"
#include <memory>

namespace simpledb {
/// Plans are similar to Scans but a plan accesses the metadata of the tables in
/// the query. A Scan accesses the table's data
///
/// When a SQL query is submitted, a database planner may create several plans
/// for the query and use the metadata of the tables in the query to choose
/// the most efficient plan. It then uses that plan's Open() method to create
/// the desired scan
class Plan {
public:
  virtual ~Plan() {}
  virtual std::shared_ptr<Scan> Open() = 0;
  virtual int BlocksAccessed() = 0;
  virtual int RecordsOutput() = 0;
  virtual int DistinctValues(const std::string &fieldname) = 0;
  virtual Schema GetSchema() = 0;
};
} // namespace simpledb
