#include "plan/productplan.hpp"
#include "plan/plan.hpp"
#include "query/productscan.hpp"
#include <memory>
namespace simpledb {
ProductPlan::ProductPlan(const std::shared_ptr<Plan> &plan1,
                         const std::shared_ptr<Plan> &plan2)
    : _plan1(plan1), _plan2(plan2) {
  _schema.AddAll(_plan1->GetSchema());
  _schema.AddAll(_plan2->GetSchema());
}

std::shared_ptr<Scan> ProductPlan::Open() {
  std::shared_ptr<Scan> scan1 = _plan1->Open();
  std::shared_ptr<Scan> scan2 = _plan2->Open();
  return std::static_pointer_cast<Scan>(
      std::make_shared<ProductScan>(scan1, scan2));
}

int ProductPlan::BlocksAccessed() {
  return _plan1->BlocksAccessed() +
         (_plan1->RecordsOutput() * _plan2->BlocksAccessed());
}

int ProductPlan::RecordsOutput() {
  return _plan1->RecordsOutput() * _plan2->RecordsOutput();
}

int ProductPlan::DistinctValues(const std::string &fieldname) {
  if (_plan1->GetSchema().HasField(fieldname)) {
    return _plan1->DistinctValues(fieldname);
  } else {
    return _plan2->DistinctValues(fieldname);
  }
}

Schema ProductPlan::GetSchema() { return _schema; }

} // namespace simpledb
