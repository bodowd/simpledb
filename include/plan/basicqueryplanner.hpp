#pragma once

#include "metadata/metadatamanager.hpp"
#include "plan/queryplanner.hpp"
#include "tx/transaction.hpp"
#include <memory>

namespace simpledb {
/// p. 279 The basic query planning algorithm is rigid and naive. It generates
/// the product plans in the order returned by the method QueryData.Tables This
/// order is completely arbitrary -- any other ordering of the tables would
/// produce an equivalent scan. This will make the performance of this algorithm
/// erratic.
/// This planner also doesn't use the plan metadata to help determine the order
/// of product plans
class BasicQueryPlanner : public QueryPlanner {
public:
  BasicQueryPlanner(MetadataManager *mm);
  std::shared_ptr<Plan> CreatePlan(QueryData *data, Transaction *tx) override;

private:
  MetadataManager *_metadata_mgr;
};
} // namespace simpledb
