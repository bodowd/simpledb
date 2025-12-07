#pragma once

#include "metadata/metadatamanager.hpp"
#include "opt/tableplanner.hpp"
#include "parse/querydata.hpp"
#include "plan/planner.hpp"
#include "plan/queryplanner.hpp"
#include <memory>
namespace simpledb {
class HeuristicQueryPlanner : public QueryPlanner {
public:
  HeuristicQueryPlanner(MetadataManager &mm);
  std::shared_ptr<Plan> CreatePlan(QueryData *data, Transaction *tx) override;
  std::shared_ptr<Plan> GetLowestSelectPlan();
  std::shared_ptr<Plan> GetLowestJoinPlan(const std::shared_ptr<Plan> &current);
  std::shared_ptr<Plan>
  GetLowestProductPlan(const std::shared_ptr<Plan> &current);
  void SetPlanner(Planner *planner);

private:
  MetadataManager &_metadata_mgr;
  std::set<std::shared_ptr<TablePlanner>> _table_planners;
};
} // namespace simpledb
