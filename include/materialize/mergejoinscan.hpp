#pragma once

#include "materialize/sortscan.hpp"
#include "query/scan.hpp"

namespace simpledb {
class MergeJoinScan : public Scan {
public:
  /// Places _scan1 and _scan2 at the record right before the first record
  MergeJoinScan(const std::shared_ptr<Scan> &scan1,
                const std::shared_ptr<SortScan> &scan2,
                const std::string &fieldName1, const std::string &fieldName2);
  void BeforeFirst() override;
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  void Close() override;

private:
  std::shared_ptr<Scan> _scan1;
  std::shared_ptr<SortScan> _scan2;
  std::string _field_name1, _field_name2;
  Constant _join_val;
};
} // namespace simpledb
