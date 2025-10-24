#include "plan/basicqueryplanner.hpp"
#include "parse/parser.hpp"
#include "plan/productplan.hpp"
#include "plan/projectplan.hpp"
#include "plan/selectplan.hpp"
#include "plan/tableplan.hpp"
#include <memory>
namespace simpledb {
BasicQueryPlanner::BasicQueryPlanner(MetadataManager *mm) : _metadata_mgr(mm) {}

std::shared_ptr<Plan> BasicQueryPlanner::CreatePlan(QueryData *data,
                                                    Transaction *tx) {
  /// Create a plan for each table or view mentioned in the query data (which
  /// comes from the parser)
  std::vector<std::shared_ptr<Plan>> plans;
  for (const std::string &tablename : data->Tables()) {
    std::string viewDef = _metadata_mgr->GetViewDef(tablename, tx);
    if (!viewDef.empty()) {
      Parser parser(viewDef);
      std::unique_ptr<QueryData> viewData = parser.Query();
      plans.emplace_back(CreatePlan(viewData.get(), tx));
    } else {
      plans.emplace_back(std::static_pointer_cast<Plan>(
          std::make_shared<TablePlan>(tx, tablename, _metadata_mgr)));
    }
  }
  /// Create the product of all table plans
  /// The basic query planning algorithm just generates the product plans in the
  /// order returned by QueryData.Tables; it starts with plans[0] and at each
  /// iteration replaces p with plans[i]
  std::shared_ptr<Plan> p = plans[0];
  for (int i = 1; i < static_cast<int>(plans.size()); i++) {
    p = std::static_pointer_cast<Plan>(
        std::make_shared<ProductPlan>(p, plans[i]));
  }
  /// Add a selection plan for the predicate in the where clause
  p = std::static_pointer_cast<Plan>(
      std::make_shared<SelectPlan>(p, data->Pred()));
  /// Project on the field names in the select class
  return std::static_pointer_cast<Plan>(
      std::make_shared<ProjectPlan>(p, data->Fields()));
}
} // namespace simpledb
