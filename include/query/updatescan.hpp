#pragma once
#include "query/scan.hpp"
#include "record/rid.hpp"
namespace simpledb {

/// pg. 220 in the book
///
/// A scan is updatable if every output record r has a corresponding record r'
/// in an underlying database table. An update to r is defined as an update to
/// r'. TableScan and SelectScan implement the UpdateScan. Since these do not
/// add or filter out records, they are updatable.
///
/// The UpdateScan class has methods that allow clients to modify records.
class UpdateScan : public Scan {
public:
  virtual ~UpdateScan() = default;
  virtual void SetInt(const std::string &fieldname, int val) = 0;
  virtual void SetString(const std::string &fieldname,
                         const std::string &val) = 0;
  virtual void SetVal(const std::string &fieldname, const Constant &val) = 0;
  virtual void Insert() = 0;
  virtual void Delete() = 0;
  // Get the RID of the current record the scan is at
  virtual RID GetRid() = 0;
  // Positions the current record at the specified RID.
  virtual void MoveToRid(const RID &rid) = 0;
};
} // namespace simpledb
