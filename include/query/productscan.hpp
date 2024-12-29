#pragma once
#include "query/constant.hpp"
#include "query/scan.hpp"
#include <memory>
namespace simpledb {

/// pg 223 in the book
///
/// Iterate through all possible combinations of records from underlying scans.
///
/// Implements a nested loop to scan through the two underlying scans.
class ProductScan : public Scan {
public:
  ProductScan(const std::shared_ptr<Scan> s1, const std::shared_ptr<Scan> s2);
  void BeforeFirst() override;

  /// Implements the nested loop
  bool Next() override;
  int GetInt(const std::string &fieldname) override;
  std::string GetString(const std::string &fieldname) override;
  Constant GetVal(const std::string &fieldname) override;
  bool HasField(const std::string &fieldname) override;
  void Close() override;

private:
  std::shared_ptr<Scan> _scan1, _scan2;
};
} // namespace simpledb
