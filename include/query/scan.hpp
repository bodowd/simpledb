#pragma once
#include "constant.hpp"
#include <string>

namespace simpledb {

/// pg. 217 in the book
///
/// A Scan is an object that represents the output of a relational algebra
/// query. A Scan object corresponds to a node in a query tree.
///
/// A query defines a virtual table, and the Scan interface has methods that
/// allow clients to read from this virtual table, but not update.
///
/// In order to do updates, see the UpdateScan class.
///
/// Each relational operator object constitute the internal nodes of a query
/// tree.
///
/// TableScan objects denote the leaves of the tree.
class Scan {
public:
  virtual ~Scan() = default;
  virtual void BeforeFirst() = 0;
  virtual bool Next() = 0;
  virtual int GetInt(const std::string &fieldname) = 0;
  virtual std::string GetString(const std::string &fieldname) = 0;
  virtual Constant GetVal(const std::string &fieldname) = 0;
  virtual bool HasField(const std::string &fieldname) = 0;
  virtual void Close() = 0;
};
} // namespace simpledb
