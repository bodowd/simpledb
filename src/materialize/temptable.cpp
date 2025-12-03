#include "materialize/temptable.hpp"
#include "record/tablescan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

TempTable::TempTable(Transaction *tx, const Schema &schema)
    : _tx(tx), _table_name(NextTableName()), _layout(schema) {}

std::shared_ptr<UpdateScan> TempTable::Open() {
  return std::static_pointer_cast<UpdateScan>(
      std::make_shared<TableScan>(_tx, _table_name, _layout));
}

std::string TempTable::TableName() { return _table_name; }

Layout TempTable::GetLayout() { return _layout; }

std::string TempTable::NextTableName() {
  std::unique_lock lock(_mutex);
  _next_table_num++;
  return "temp" + std::to_string(_next_table_num);
}

} // namespace simpledb
