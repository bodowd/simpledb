#include "materialize/mergejoinplan.hpp"
#include "materialize/mergejoinscan.hpp"
#include "materialize/sortplan.hpp"
#include <memory>
#include <vector>
namespace simpledb {

MergeJoinPlan::MergeJoinPlan(Transaction *tx,
                             const std::shared_ptr<Plan> &plan1,
                             const std::shared_ptr<Plan> &plan2,
                             const std::string &fieldName1,
                             const std::string &fieldName2)
    : _field_name1(fieldName1), _field_name2(fieldName2) {
  std::vector<std::string> sortList1 = {_field_name1};
  _plan1 = std::static_pointer_cast<Plan>(
      std::make_shared<SortPlan>(tx, plan1, sortList1));
  std::vector<std::string> sortList2 = {_field_name2};
  _plan2 = std::static_pointer_cast<Plan>(
      std::make_shared<SortPlan>(tx, plan2, sortList2));
  _sch.AddAll(_plan1->GetSchema());
  _sch.AddAll(_plan2->GetSchema());
}

std::shared_ptr<Scan> MergeJoinPlan::Open() {
  std::shared_ptr<Scan> s1 = _plan1->Open();
  auto s2 = std::static_pointer_cast<SortScan>(_plan2->Open());
  return std::static_pointer_cast<Scan>(
      std::make_shared<MergeJoinScan>(s1, s2, _field_name1, _field_name2));
}

int MergeJoinPlan::BlocksAccessed() {
  return _plan1->BlocksAccessed() + _plan2->BlocksAccessed();
}

int MergeJoinPlan::RecordsOutput() {
  int maxVals = std::max(_plan1->DistinctValues(_field_name1),
                         _plan2->DistinctValues(_field_name2));
  return (_plan1->RecordsOutput() * _plan2->RecordsOutput()) / maxVals;
}

int MergeJoinPlan::DistinctValues(const std::string &fieldName) {
  if (_plan1->GetSchema().HasField(fieldName)) {
    return _plan1->DistinctValues(fieldName);
  } else {
    return _plan2->DistinctValues(fieldName);
  }
}

Schema MergeJoinPlan::GetSchema() { return _sch; }
} // namespace simpledb
