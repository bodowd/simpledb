#pragma once

#include "index/index.hpp"
#include "record/tablescan.hpp"
#include <memory>

namespace simpledb {

/// pg. 349 in the book
class IndexSelectScan : public Scan {
public:
  IndexSelectScan(const std::shared_ptr<TableScan> &tableScan,
                  const std::shared_ptr<Index> &index, const Constant &val);
  void BeforeFirst() override;
  /// If there is more data in the index matching the search key, position the
  /// table scan to this record. Note the table is never scanned. The current
  /// record is always obtained via the data record id of an index record
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  void Close() override;

private:
  std::shared_ptr<TableScan> _table_scan;
  std::shared_ptr<Index> _index;
  Constant _val;
};
} // namespace simpledb
