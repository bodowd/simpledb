#include "index/query/indexselectscan.hpp"
namespace simpledb {
IndexSelectScan::IndexSelectScan(const std::shared_ptr<TableScan> &tableScan,
                                 const std::shared_ptr<Index> &index,
                                 const Constant &val)
    : _table_scan(tableScan), _index(index), _val(val) {}

void IndexSelectScan::BeforeFirst() { _index->BeforeFirst(_val); }

bool IndexSelectScan::Next() {
  bool isData = _index->Next();
  if (isData) {
    RID r = _index->GetDataRid();
    _table_scan->MoveToRid(r);
  }
  return isData;
}

int IndexSelectScan::GetInt(const std::string &fieldName) {
  return _table_scan->GetInt(fieldName);
}

std::string IndexSelectScan::GetString(const std::string &fieldName) {
  return _table_scan->GetString(fieldName);
}

Constant IndexSelectScan::GetVal(const std::string &fieldName) {
  return _table_scan->GetVal(fieldName);
}

bool IndexSelectScan::HasField(const std::string &fieldName) {
  return _table_scan->HasField(fieldName);
}

void IndexSelectScan::Close() {
  _index->Close();
  _table_scan->Close();
}

} // namespace simpledb
