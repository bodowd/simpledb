#pragma once

#include "plan/plan.hpp"
#include "tx/transaction.hpp"
#include <memory>

namespace simpledb {
/// p. 367 in the book
/// MaterializePlan implements the materialize operator. The materialize
/// operator saves the output of a subquery in a temporary table. This caches
/// those records so that they do not need to be computed multiple times
class MaterializePlan : public Plan {
public:
  MaterializePlan(Transaction *tx, const std::shared_ptr<Plan> &sourcePlan);
  /// Preprocess the input. Creates a new temp table, copies the input records
  /// into the table scan for the temp table and returns the temp table table
  /// scan
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  Transaction *_tx;
  std::shared_ptr<Plan> _source_plan;
};
} // namespace simpledb
