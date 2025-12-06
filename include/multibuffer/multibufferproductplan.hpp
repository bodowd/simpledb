#pragma once

#include "materialize/temptable.hpp"
#include "plan/plan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

/// p. 407 in the book
/// Implements the multibuffer product algorithm
class MultiBufferProductPlan : public Plan {
public:
  MultiBufferProductPlan(Transaction &tx,
                         const std::shared_ptr<Plan> &leftHandSide,
                         const std::shared_ptr<Plan> &rightHandSide);
  /// Materialize both the left and right hand side records
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  Transaction &_tx;
  std::shared_ptr<Plan> _left_hand_side, _right_hand_side;
  Schema _schema;

  std::shared_ptr<TempTable> copyRecordsFrom(Plan &plan);
};
} // namespace simpledb
