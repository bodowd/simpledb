#pragma once
#include "record/layout.hpp"
#include "tx/transaction.hpp"

namespace simpledb {
/// pg. 194 in the book
/// Manages the table data.
class TableManager {
public:
  const std::string TABLENAME = "tblname";
  const std::string SLOTSIZE = "slotsize";
  /// name of table catalog table
  const std::string TABLECAT = "tblcat";
  /// name of field catalog table
  const std::string FIELDCAT = "fldcat";

  TableManager(bool isNew, Transaction *tx);
  /// Takes the table's name and schema as arguments, calculates the record
  /// offsets and saves it all in the catalog.
  void CreateTable(const std::string &tablename, const Schema &sch,
                   Transaction *tx);
  /// Goes to the catalog and extracts metadata for the specified table. Returns
  /// a Layout object containing the metadata
  Layout GetLayout(const std::string &tablename, Transaction *tx);
  const int MAX_NAME = 16;

private:
  // Table catalog layout
  Layout _tcatLayout;
  // Field catalog layout
  Layout _fcatLayout;
};
} // namespace simpledb
