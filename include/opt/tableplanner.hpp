#pragma once

#include "metadata/metadatamanager.hpp"
#include "plan/tableplan.hpp"
#include "query/predicate.hpp"
#include "tx/transaction.hpp"
#include <memory>

namespace simpledb {
/// p 445 in the book
///
/// Creates a plan for a single table. Adds selection and join predicates, and
/// uses indexes when possible
class TablePlanner {
public:
  TablePlanner(const std::string &tableName, const Predicate &pred,
               Transaction &tx, MetadataManager &mm);
  std::shared_ptr<Plan> MakeSelectPlan();
  /// Determine if a join exists between the specified plan (in _plan and which
  /// _schema refers to) and the current plan being considered. If no join
  /// predicate exists, returns nullptr. If a join predicate exists, tries to
  /// see if an IndexJoinScan can be created. If not, uses a multibuffer product
  /// followed by select
  std::shared_ptr<Plan> MakeJoinPlan(const std::shared_ptr<Plan> &current);
  std::shared_ptr<Plan> MakeProductPlan(const std::shared_ptr<Plan> &current);

private:
  Predicate _predicate;
  Transaction &_tx;
  std::shared_ptr<TablePlan> _plan;
  Schema _schema;
  std::map<std::string, IndexInfo> _indexes;

  /// If an index can be used to do the select, returns an IndexSelect plan.
  /// Otherwise, returns nullptr
  std::shared_ptr<Plan> makeIndexSelect();
  std::shared_ptr<Plan> makeIndexJoin(const std::shared_ptr<Plan> &current,
                                      const Schema &currentSchema);
  std::shared_ptr<Plan> makeProductJoin(const std::shared_ptr<Plan> &current,
                                        const Schema &currentSchema);
  /// Determine the portion of the predicate that applies to the table and
  /// create a select plan for it
  std::shared_ptr<Plan> addSelectPred(const std::shared_ptr<Plan> &plan);
  std::shared_ptr<Plan> addJoinPred(const std::shared_ptr<Plan> &plan,
                                    const Schema &currentSchema);
};
} // namespace simpledb
