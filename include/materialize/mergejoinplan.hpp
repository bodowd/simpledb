#pragma once

#include "plan/plan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

class MergeJoinPlan : public Plan {
public:
  MergeJoinPlan(Transaction *tx, const std::shared_ptr<Plan> &plan1,
                const std::shared_ptr<Plan> &plan2,
                const std::string &fieldName1, const std::string &fieldName2);
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan1, _plan2;
  std::string _field_name1, _field_name2;
  Schema _sch;
};
} // namespace simpledb
