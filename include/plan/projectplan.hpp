#pragma once

#include "plan/plan.hpp"
#include <memory>
namespace simpledb {

class ProjectPlan : public Plan {
public:
  ProjectPlan(const std::shared_ptr<Plan> &plan,
              const std::vector<std::string> &fieldlist);
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldname) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan;
  Schema _schema;
};
} // namespace simpledb
