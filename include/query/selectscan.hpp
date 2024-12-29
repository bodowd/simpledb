#pragma once
#include "predicate.hpp"
#include "updatescan.hpp"
namespace simpledb {

/// pg 221 in the book
///
/// Select scans look for a record that satisfies the predicate.
class SelectScan : public UpdateScan {
public:
  SelectScan(std::shared_ptr<Scan> s, const Predicate &pred);

  /// scan methods
  void BeforeFirst() override;
  /// Loop through the underlying scan to look for a record that satisfies the
  /// predicate. If such a record is found, it becomes the current record and
  /// the method returns true. This record identifier can be obtained by
  /// GetRid(). Otherwise, if no record is found, the method
  /// returns false.
  bool Next() override;
  int GetInt(const std::string &fieldname) override;
  std::string GetString(const std::string &fieldname) override;
  Constant GetVal(const std::string &fieldname) override;
  bool HasField(const std::string &fieldname) override;
  void Close() override;

  /// update scan methods
  void SetInt(const std::string &fieldname, int val) override;
  void SetString(const std::string &fieldname, const std::string &val) override;
  void SetVal(const std::string &fieldname, const Constant &val) override;
  void Insert() override;
  void Delete() override;
  // Get the RID of the current record the scan is at
  RID GetRid() override;
  // Positions the current record at the specified RID.
  void MoveToRid(const RID &rid) override;

private:
  /// This holds the scan of its underlying input table.
  /// A scan's current record is the same as the current record of its
  /// underlying scan.
  ///
  /// Select scans are updatable, so they inherit from UpdateScan.
  /// The UpdateScan assumes the underlying scan is also updatable. In
  /// particular, they assume that they can cast the underlying scan to
  /// UpdateScan without an exception
  std::shared_ptr<Scan> _scan;
  Predicate _predicate;

  std::shared_ptr<UpdateScan> castScan(std::shared_ptr<Scan> s);
};
} // namespace simpledb
