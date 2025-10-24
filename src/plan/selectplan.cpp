#include "plan/selectplan.hpp"
#include "query/predicate.hpp"
#include "query/selectscan.hpp"
#include <memory>

namespace simpledb {
SelectPlan::SelectPlan(const std::shared_ptr<Plan> &plan, const Predicate &pred)
    : _plan(plan), _predicate(pred) {}

std::shared_ptr<Scan> SelectPlan::Open() {
  std::shared_ptr<Scan> s = _plan->Open();
  return std::static_pointer_cast<Scan>(
      std::make_shared<SelectScan>(s, _predicate));
}

int SelectPlan::BlocksAccessed() { return _plan->BlocksAccessed(); }

int SelectPlan::RecordsOutput() {
  /// ReductionFactor is used to calculate the extent to which the predicate
  /// reduces the size of the input table
  return _plan->RecordsOutput() / _predicate.ReductionFactor(_plan.get());
}

int SelectPlan::DistinctValues(const std::string &fieldname) {
  if (!_predicate.EquatesWithConstant(fieldname).IsNull()) {
    return 1;
  } else {
    std::string fname = _predicate.EquatesWithField(fieldname);
    if (!fname.empty()) {
      return std::min(_plan->DistinctValues(fname),
                      _plan->DistinctValues(fname));
    } else {
      return _plan->DistinctValues(fname);
    }
  }
}

Schema SelectPlan::GetSchema() { return _plan->GetSchema(); }
} // namespace simpledb
