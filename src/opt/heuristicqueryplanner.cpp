#include "opt/heuristicqueryplanner.hpp"
#include "metadata/metadatamanager.hpp"
#include "plan/projectplan.hpp"
#include <memory>
#include <stdexcept>
namespace simpledb {
HeuristicQueryPlanner::HeuristicQueryPlanner(MetadataManager &mm)
    : _metadata_mgr(mm) {}

std::shared_ptr<Plan> HeuristicQueryPlanner::CreatePlan(QueryData *data,
                                                        Transaction *tx) {
  /// Create a TablePlanner for each table mentioned in the query
  for (const std::string &tableName : data->Tables()) {
    auto p = std::make_shared<TablePlanner>(tableName, data->Pred(), *tx,
                                            _metadata_mgr);
    _table_planners.insert(p);
  }

  /// Choose the plan with the lowest records output to begin the join order
  std::shared_ptr<Plan> currentPlan = GetLowestSelectPlan();

  /// repeatedly add a plan to the join order until all tables are accounted for
  while (!_table_planners.empty()) {
    std::shared_ptr<Plan> p = GetLowestJoinPlan(currentPlan);
    if (p) {
      currentPlan = p;
    } else {
      /// no applicable join
      currentPlan = GetLowestProductPlan(currentPlan);
    }
  }

  /// project on the field names and return the plan
  return std::static_pointer_cast<Plan>(
      std::make_shared<ProjectPlan>(currentPlan, data->Fields()));
}

std::shared_ptr<Plan> HeuristicQueryPlanner::GetLowestSelectPlan() {
  std::shared_ptr<TablePlanner> bestTablePlan;
  std::shared_ptr<Plan> bestPlan;
  for (const auto &tablePlan : _table_planners) {
    std::shared_ptr<Plan> p = tablePlan->MakeSelectPlan();
    if (!bestPlan || p->RecordsOutput() < bestPlan->RecordsOutput()) {
      bestTablePlan = tablePlan;
      bestPlan = p;
    }
  }
  _table_planners.erase(bestTablePlan);
  return bestPlan;
}

std::shared_ptr<Plan>
HeuristicQueryPlanner::GetLowestJoinPlan(const std::shared_ptr<Plan> &current) {
  std::shared_ptr<TablePlanner> bestTablePlan;
  std::shared_ptr<Plan> bestPlan;
  for (const auto &tablePlan : _table_planners) {
    std::shared_ptr<Plan> p = tablePlan->MakeJoinPlan(current);
    if (p && (!bestPlan || p->RecordsOutput() < bestPlan->RecordsOutput())) {
      bestTablePlan = tablePlan;
      bestPlan = p;
    }
  }
  if (bestPlan) {
    /// leave the other table plans in _table_planners for the planner to
    /// consider for the next step of the plan
    _table_planners.erase(bestTablePlan);
  }
  return bestPlan;
}

std::shared_ptr<Plan> HeuristicQueryPlanner::GetLowestProductPlan(
    const std::shared_ptr<Plan> &current) {
  std::shared_ptr<TablePlanner> bestTablePlan;
  std::shared_ptr<Plan> bestPlan;
  for (const auto &tablePlan : _table_planners) {
    std::shared_ptr<Plan> p = tablePlan->MakeProductPlan(current);
    if (!bestPlan || p->RecordsOutput() < bestPlan->RecordsOutput()) {
      bestTablePlan = tablePlan;
      bestPlan = p;
    }
  }
  /// leave the other table plans in _table_planners for the planner to consider
  /// for the next step of the plan
  _table_planners.erase(bestTablePlan);
  return bestPlan;
}

void HeuristicQueryPlanner::SetPlanner(Planner *planner) {
  throw std::runtime_error("SetPlanner not implemented yet");
}

} // namespace simpledb
