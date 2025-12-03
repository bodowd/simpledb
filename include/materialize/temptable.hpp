#pragma once
#include "query/updatescan.hpp"
#include "record/layout.hpp"
#include "tx/transaction.hpp"
namespace simpledb {

/// p. 364 in the book
/// Materialized implementations store their input records in temporary tables.
/// A temp table differs from regular tables in three ways:
/// 1. Each temp table manages its own metadata and has its own GetLayout method
/// 2. The file manager deletes the tables during system initialization
/// 3. The recovery manager does not log changes to temp tables
class TempTable {
public:
  TempTable(Transaction *tx, const Schema &schema);
  std::shared_ptr<UpdateScan> Open();
  std::string TableName();
  Layout GetLayout();
  std::string NextTableName();

private:
  static int _next_table_num;
  static std::mutex _mutex;
  Transaction *_tx;
  std::string _table_name;
  Layout _layout;
};
} // namespace simpledb
