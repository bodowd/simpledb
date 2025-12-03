#pragma once

#include "materialize/aggregationfn.hpp"
#include "plan/plan.hpp"
#include "tx/transaction.hpp"
#include <memory>
#include <vector>
namespace simpledb {
class GroupByPlan : public Plan {
public:
  GroupByPlan(
      Transaction *tx, const std::shared_ptr<Plan> plan,
      const std::vector<std::string> &groupFields,
      const std::vector<std::shared_ptr<AggregationFn>> &aggregationFunctions);
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan;
  std::vector<std::string> _group_fields;
  std::vector<std::shared_ptr<AggregationFn>> _aggregation_functions;
  Schema _sch;
};
} // namespace simpledb
