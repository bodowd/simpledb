#pragma once

#include "metadata/metadatamanager.hpp"
#include "metadata/statmanager.hpp"
#include "plan/plan.hpp"
#include "record/layout.hpp"
#include "tx/transaction.hpp"

namespace simpledb {
class TablePlan : public Plan {
public:
  TablePlan(Transaction *tx, const std::string &tablename, MetadataManager *mm);
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldname) override;
  Schema GetSchema() override;

private:
  Transaction *_tx;
  std::string _tablename;
  Layout _layout;
  StatInfo _statinfo;
};
} // namespace simpledb
