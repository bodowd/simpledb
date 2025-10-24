#include "plan/projectplan.hpp"
#include "query/projectscan.hpp"
#include <memory>
namespace simpledb {
ProjectPlan::ProjectPlan(const std::shared_ptr<Plan> &plan,
                         const std::vector<std::string> &fieldlist)
    : _plan(plan) {
  for (const std::string &fieldname : fieldlist) {
    _schema.Add(fieldname, _plan->GetSchema());
  }
}

std::shared_ptr<Scan> ProjectPlan::Open() {
  std::shared_ptr<Scan> s = _plan->Open();
  return std::static_pointer_cast<Scan>(
      std::make_shared<ProjectScan>(s, _schema.Fields()));
}

int ProjectPlan::BlocksAccessed() { return _plan->RecordsOutput(); }

int ProjectPlan::RecordsOutput() { return _plan->RecordsOutput(); }

int ProjectPlan::DistinctValues(const std::string &fieldname) {
  return _plan->DistinctValues(fieldname);
}

Schema ProjectPlan::GetSchema() { return _schema; }

} // namespace simpledb
