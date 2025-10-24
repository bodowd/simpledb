#include "plan/tableplan.hpp"
#include "metadata/metadatamanager.hpp"
#include "record/tablescan.hpp"
#include <memory>
namespace simpledb {
TablePlan::TablePlan(Transaction *tx, const std::string &tablename,
                     MetadataManager *mm)
    : _tx(tx), _tablename(tablename) {
  _layout = mm->GetLayout(tablename, tx);
  _statinfo = mm->GetStatInfo(tablename, _layout, tx);
}

std::shared_ptr<Scan> TablePlan::Open() {
  return std::static_pointer_cast<Scan>(
      std::make_shared<TableScan>(_tx, _tablename, _layout));
}

int TablePlan::BlocksAccessed() { return _statinfo.BlocksAccessed(); }

int TablePlan::RecordsOutput() { return _statinfo.RecordsOutput(); }

int TablePlan::DistinctValues(const std::string &fieldname) {
  return _statinfo.DistinctValues(fieldname);
}

Schema TablePlan::GetSchema() { return _layout.GetSchema(); }

} // namespace simpledb
