#pragma once

#include "metadata/indexmanager.hpp"
#include "plan/plan.hpp"

namespace simpledb {

class IndexSelectPlan : public Plan {
public:
  IndexSelectPlan(const std::shared_ptr<Plan> &plan, const IndexInfo &indexInfo,
                  const Constant &val);
  /// Open the index and pass the plan to the IndexSelectScan constructor
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan;
  IndexInfo _index_info;
  Constant _val;
};
} // namespace simpledb
