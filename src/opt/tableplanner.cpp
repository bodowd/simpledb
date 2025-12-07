#include "opt/tableplanner.hpp"
#include "index/planner/indexjoinplan.hpp"
#include "index/planner/indexselectplan.hpp"
#include "metadata/metadatamanager.hpp"
#include "multibuffer/multibufferproductplan.hpp"
#include "plan/selectplan.hpp"
#include "query/predicate.hpp"
#include "tx/transaction.hpp"
#include <iostream>
#include <memory>

namespace simpledb {

TablePlanner::TablePlanner(const std::string &tableName, const Predicate &pred,
                           Transaction &tx, MetadataManager &mm)
    : _predicate(pred), _tx(tx) {
  _plan = std::make_shared<TablePlan>(&_tx, tableName, &mm);
  _schema = _plan->GetSchema();
  _indexes = mm.GetIndexInfo(tableName, &_tx);
}

std::shared_ptr<Plan> TablePlanner::MakeSelectPlan() {
  /// check if an index can be used, if not use the default TablePlan
  std::shared_ptr<Plan> p = makeIndexSelect();
  if (!p) {
    p = _plan;
  }

  return addSelectPred(p);
}

std::shared_ptr<Plan>
TablePlanner::MakeJoinPlan(const std::shared_ptr<Plan> &current) {
  Schema currSchema = current->GetSchema();
  Predicate joinPred = _predicate.JoinSubPred(_schema, currSchema);
  if (joinPred.IsNull()) {
    return nullptr;
  }
  std::shared_ptr<Plan> p = makeIndexJoin(current, currSchema);
  /// If an index join is not possible, does a multibuffer product followed by
  /// select
  if (!p) {
    p = makeProductJoin(current, currSchema);
  }
  return p;
}

std::shared_ptr<Plan>
TablePlanner::MakeProductPlan(const std::shared_ptr<Plan> &current) {
  std::shared_ptr<Plan> p = addSelectPred(_plan);
  return std::static_pointer_cast<Plan>(
      std::make_shared<MultiBufferProductPlan>(_tx, current, p));
}

std::shared_ptr<Plan> TablePlanner::makeIndexSelect() {
  for (const auto &[fieldName, index] : _indexes) {
    Constant val = _predicate.EquatesWithConstant(fieldName);
    if (!val.IsNull()) {
      std::cout << "index on " + fieldName + " used" << std::endl;
      return std::static_pointer_cast<Plan>(
          std::make_shared<IndexSelectPlan>(_plan, index, val));
    }
  }
  return nullptr;
}

std::shared_ptr<Plan>
TablePlanner::makeIndexJoin(const std::shared_ptr<Plan> &current,
                            const Schema &currentSchema) {
  for (const auto &[fieldName, index] : _indexes) {
    std::string outerField = _predicate.EquatesWithField(fieldName);
    if (!outerField.empty() && currentSchema.HasField(outerField)) {
      auto p = std::static_pointer_cast<Plan>(
          std::make_shared<IndexJoinPlan>(current, _plan, index, outerField));
      p = addSelectPred(p);
      return addJoinPred(p, currentSchema);
    }
  }
  return nullptr;
}

std::shared_ptr<Plan>
TablePlanner::makeProductJoin(const std::shared_ptr<Plan> &current,
                              const Schema &currentSchema) {
  std::shared_ptr<Plan> p = MakeProductPlan(current);
  return addJoinPred(p, currentSchema);
}

std::shared_ptr<Plan>
TablePlanner::addSelectPred(const std::shared_ptr<Plan> &plan) {
  Predicate selectPred = _predicate.SelectSubPred(_schema);
  if (!selectPred.IsNull()) {
    return std::static_pointer_cast<Plan>(
        std::make_shared<SelectPlan>(plan, selectPred));
  } else {
    return plan;
  }
}

std::shared_ptr<Plan>
TablePlanner::addJoinPred(const std::shared_ptr<Plan> &plan,
                          const Schema &currentSchema) {
  Predicate joinPred = _predicate.JoinSubPred(currentSchema, _schema);
  if (!joinPred.IsNull()) {
    return std::static_pointer_cast<Plan>(
        std::make_shared<SelectPlan>(plan, joinPred));
  } else {
    return plan;
  }
}

} // namespace simpledb
