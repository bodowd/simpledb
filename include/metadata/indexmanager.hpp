#pragma once
#include "index/index.hpp"
#include "metadata/statmanager.hpp"
#include "metadata/tablemangager.hpp"
#include "record/schema.hpp"
#include "tx/transaction.hpp"
#include <map>
#include <memory>
#include <string>
namespace simpledb {
class IndexManager;
/// pg. 199 in the book describes the IndexInfo class
///
/// It provides statistical information about a chosen index.
class IndexInfo {
public:
  IndexInfo();
  IndexInfo(const IndexInfo &ii);
  IndexInfo(const std::string &idxname, const std::string &fieldname,
            const std::string &idxtype, const Schema &tableschema,
            Transaction *tx, const StatInfo &si);

  IndexInfo &operator=(const IndexInfo &ii);
  std::shared_ptr<Index> Open();

  int BlocksAccessed();
  int RecordsOutput();
  int DistinctValues(const std::string &fieldname);
  std::string IndexType();

private:
  Layout createIdxLayout();
  std::string _idxname, _fieldname, _idxtype;
  Transaction *_tx;
  Schema _tblSchema;
  Layout _idxLayout;
  StatInfo _si;
};

/// pg. 203 in the book
///
/// The IndexManager stores and retrieves information on the metadata for an
/// index. It contains the name of the index, the name of the table it is
/// indexing, and the list of its indexed fields.
///
/// It stores index metadata in the catalog table "idxcat". This table has one
/// record for each index and three fields.
///
/// 1. The name of the index
/// 2. The name of the table being indexed
/// 3. The name of the indexed field
class IndexManager : public std::enable_shared_from_this<IndexManager> {
  friend class IndexInfo;

public:
  IndexManager(bool isNew, TableManager *tm, StatManager *sm, Transaction *tx);
  /// Inserts a new record into the "idxcat" table in the catalog in order to
  /// add an index.
  void CreateIndex(const std::string &idxname, const std::string &tablename,
                   const std::string &fieldname, const std::string &indextype,
                   Transaction *tx);
  /// Searches the "idxcat" catalog table for records having the specified
  /// tablename and returns the indexes of that table in a std::map.
  std::map<std::string, IndexInfo> GetIndexInfo(const std::string &tablename,
                                                Transaction *tx) const;

private:
  TableManager *_tm;
  StatManager *_sm;
  Layout _layout;
};
} // namespace simpledb
