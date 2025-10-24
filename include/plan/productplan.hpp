#pragma once

#include "plan/plan.hpp"
#include <memory>
namespace simpledb {

/// p. 278
class ProductPlan : public Plan {
public:
  ProductPlan(const std::shared_ptr<Plan> &plan1,
              const std::shared_ptr<Plan> &plan2);
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldname) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan1, _plan2;
  Schema _schema;
};
} // namespace simpledb
