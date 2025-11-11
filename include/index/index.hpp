#pragma once

#include "query/constant.hpp"
#include "record/rid.hpp"
namespace simpledb {

/// The Index Class or interface allows clients to interact with any index
/// structure in the same way. SimpleDB supports a B-Tree and Hash Index
class Index {
public:
  virtual ~Index() {};
  /// Move to the record right before the first record that matches the search
  /// key so that a call to Next() will give the first matching record
  virtual void BeforeFirst(const Constant &searchKey) = 0;
  /// Returns true if there is still a record that matches the search key
  virtual bool Next() = 0;
  /// Get the data record id of the record that matches the search key
  virtual RID GetDataRid() = 0;
  /// Each record of the index contains information about the record id (RID)
  /// The block number (as field "block"), RID (as field "id"), and the value of
  /// the record (as field "dataval") are stored
  virtual void Insert(const Constant &dataVal, const RID &dataRID) = 0;
  virtual void Remove(const Constant &dataVal, const RID &dataRID) = 0;
  virtual void Close() = 0;
};
} // namespace simpledb
