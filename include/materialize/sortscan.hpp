#pragma once

#include "materialize/recordcomparator.hpp"
#include "materialize/temptable.hpp"
#include "query/updatescan.hpp"
#include <memory>
namespace simpledb {
class SortScan : public Scan {
public:
  /// Open the temp tables (currently expects 1 or 2 runs) and move to their
  /// first record
  SortScan(const std::vector<std::shared_ptr<TempTable>> &runs,
           RecordComparator &comp);
  void BeforeFirst() override;
  /// move to the next record of the current scan and choose the lowest value
  /// record from the two scans
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  void Close() override;

  /// Save the positions of each scan
  void SavePosition();
  /// Restore the positions of each scan
  void RestorePosition();

private:
  /// _current_scan points to the scan containing the most recent record in the
  /// merge
  std::shared_ptr<UpdateScan> _scan1, _scan2, _current_scan;
  RecordComparator &_comp;
  /// indicates if there are more records left in the scan
  bool _has_more1, _has_more2;
  std::vector<RID> _saved_position;
};
} // namespace simpledb
