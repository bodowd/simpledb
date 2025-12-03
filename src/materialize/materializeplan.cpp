#include "materialize/materializeplan.hpp"
#include "materialize/temptable.hpp"
#include <cmath>
#include <memory>

namespace simpledb {

MaterializePlan::MaterializePlan(Transaction *tx,
                                 const std::shared_ptr<Plan> &sourcePlan)
    : _tx(tx), _source_plan(sourcePlan) {}

std::shared_ptr<Scan> MaterializePlan::Open() {
  Schema sch = _source_plan->GetSchema();
  std::shared_ptr<Scan> src = _source_plan->Open();

  /// create new temp table and copy records from the input table (src Scan)
  /// into the temp table
  TempTable temp(_tx, sch);
  std::shared_ptr<UpdateScan> dest = temp.Open();
  while (src->Next()) {
    dest->Insert();
    for (const std::string &fieldname : sch.Fields()) {
      dest->SetVal(fieldname, src->GetVal(fieldname));
    }
  }
  src->Close();
  dest->BeforeFirst();
  return std::static_pointer_cast<Scan>(dest);
}

int MaterializePlan::BlocksAccessed() {
  Layout layout(_source_plan->GetSchema());
  int recordsPerBlock = (_tx->BlockSize() / layout.SlotSize());
  return std::ceil(static_cast<double>(_source_plan->RecordsOutput()) /
                   static_cast<double>(recordsPerBlock));
}

int MaterializePlan::RecordsOutput() {
  /// since the source table is copied to the temp table, the records output are
  /// the same as the source table's records output
  return _source_plan->RecordsOutput();
}

int MaterializePlan::DistinctValues(const std::string &fieldname) {
  return _source_plan->DistinctValues(fieldname);
}

Schema MaterializePlan::GetSchema() { return _source_plan->GetSchema(); }

} // namespace simpledb
