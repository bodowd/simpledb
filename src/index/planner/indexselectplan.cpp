#include "index/planner/indexselectplan.hpp"
#include "index/query/indexselectscan.hpp"
#include "record/tablescan.hpp"
#include <memory>
#include <stdexcept>
namespace simpledb {

IndexSelectPlan::IndexSelectPlan(const std::shared_ptr<Plan> &plan,
                                 const IndexInfo &indexInfo,
                                 const Constant &val)
    : _plan(plan), _index_info(indexInfo), _val(val) {}

std::shared_ptr<Scan> IndexSelectPlan::Open() {
  std::shared_ptr<TableScan> ts =
      std::dynamic_pointer_cast<TableScan>(_plan->Open());
  if (!ts) {
    throw std::runtime_error("type conversion failed of Scan to TableScan");
  }
  std::shared_ptr<Index> idx = _index_info.Open();
  return std::static_pointer_cast<Scan>(
      std::make_shared<IndexSelectScan>(ts, idx, _val));
}

int IndexSelectPlan::BlocksAccessed() {
  return _index_info.BlocksAccessed() + RecordsOutput();
}

int IndexSelectPlan::RecordsOutput() { return _index_info.RecordsOutput(); }

int IndexSelectPlan::DistinctValues(const std::string &fieldName) {
  return _index_info.DistinctValues(fieldName);
}

Schema IndexSelectPlan::GetSchema() { return _plan->GetSchema(); }

} // namespace simpledb
