#pragma once

#include "plan/queryplanner.hpp"
#include "plan/updateplanner.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

/// p286 in the book
/// The planner transforms a SQL statement into a query plan
class Planner {
public:
  Planner(std::unique_ptr<QueryPlanner> qp, std::unique_ptr<UpdatePlanner> up);
  /// Uses the parser to create a query plan from the input string
  std::shared_ptr<Plan> CreateQueryPlan(const std::string &cmd,
                                        Transaction *tx);
  int ExecuteUpdate(const std::string &cmd, Transaction *tx);

private:
  std::unique_ptr<QueryPlanner> _query_planner;
  std::unique_ptr<UpdatePlanner> _update_planner;
};
} // namespace simpledb
