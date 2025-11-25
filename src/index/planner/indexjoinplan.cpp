
#include "index/planner/indexjoinplan.hpp"
#include "index/query/indexjoinscan.hpp"
#include "record/tablescan.hpp"
#include <memory>
#include <stdexcept>
namespace simpledb {
IndexJoinPlan::IndexJoinPlan(const std::shared_ptr<Plan> &plan1,
                             const std::shared_ptr<Plan> &plan2,
                             const IndexInfo &indexInfo,
                             const std::string &joinField)
    : _plan_1(plan1), _plan_2(plan2), _index_info(indexInfo),
      _join_field(joinField) {
  _sch.AddAll(_plan_1->GetSchema());
  _sch.AddAll(_plan_2->GetSchema());
}

std::shared_ptr<Scan> IndexJoinPlan::Open() {
  std::shared_ptr<Scan> s = _plan_1->Open();
  std::shared_ptr<TableScan> ts =
      std::dynamic_pointer_cast<TableScan>(_plan_2->Open());
  if (!ts) {
    throw std::runtime_error("Could not cast plan 2 to a table scan");
  }
  std::shared_ptr<Index> idx = _index_info.Open();
  return std::static_pointer_cast<Scan>(
      std::make_shared<IndexJoinScan>(s, idx, _join_field, ts));
}

int IndexJoinPlan::BlocksAccessed() {
  return _plan_1->BlocksAccessed() +
         _plan_1->RecordsOutput() * _index_info.BlocksAccessed() +
         RecordsOutput();
}

int IndexJoinPlan::RecordsOutput() {
  return _plan_1->RecordsOutput() * _index_info.RecordsOutput();
}

int IndexJoinPlan::DistinctValues(const std::string &fieldName) {
  if (_plan_1->GetSchema().HasField(fieldName)) {
    return _plan_1->DistinctValues(fieldName);
  } else {
    return _plan_2->DistinctValues(fieldName);
  }
}

Schema IndexJoinPlan::GetSchema() { return _sch; }

} // namespace simpledb
