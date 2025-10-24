#pragma once

#include "parse/querydata.hpp"
#include "tx/transaction.hpp"

namespace simpledb {
class QueryPlanner {
public:
  virtual ~QueryPlanner() {}
  virtual std::shared_ptr<Plan> CreatePlan(QueryData *qd, Transaction *tx) = 0;
};
} // namespace simpledb
