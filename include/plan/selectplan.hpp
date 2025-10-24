#pragma once

#include "query/predicate.hpp"
namespace simpledb {
/// p. 275
class SelectPlan : public Plan {
public:
  /// Chain/nest query execution plan
  /// The SelectPlan constructor takes a plan as one of its arguments making the
  /// input plan the child for the SelectPlan
  /// The SelectPlan now holds a shared_ptr to the input &plan, preventing its
  /// destruction
  SelectPlan(const std::shared_ptr<Plan> &plan, const Predicate &pred);
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  /// Cost estimation for Select plans depends on the predicate.
  int DistinctValues(const std::string &fieldname) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan;
  Predicate _predicate;
};
} // namespace simpledb
