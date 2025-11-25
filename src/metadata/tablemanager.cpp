#include "metadata/tablemangager.hpp"
#include "record/layout.hpp"
#include "record/schema.hpp"
#include "record/tablescan.hpp"
#include "tx/transaction.hpp"
#include <map>
#include <stdexcept>
namespace simpledb {
TableManager::TableManager(bool isNew, Transaction *tx) {
  Schema tcatSchema;
  tcatSchema.AddStringField(TABLENAME, MAX_NAME);
  tcatSchema.AddIntField(SLOTSIZE);
  _tcatLayout = Layout(tcatSchema);

  Schema fcatSchema;
  fcatSchema.AddStringField(TABLENAME, MAX_NAME);
  fcatSchema.AddStringField("fldname", MAX_NAME);
  fcatSchema.AddIntField("type");
  fcatSchema.AddIntField("length");
  fcatSchema.AddIntField("offset");
  _fcatLayout = Layout(fcatSchema);
  if (isNew) {
    CreateTable(TABLECAT, tcatSchema, tx);
    CreateTable(FIELDCAT, fcatSchema, tx);
  }
}

void TableManager::CreateTable(const std::string &tablename, const Schema &sch,
                               Transaction *tx) {
  /// There is one record in TABLECAT for each database table
  Layout layout = Layout(sch);

  /// insert one record into tblcat
  TableScan tcat = TableScan(tx, TABLECAT, _tcatLayout);
  tcat.Insert();
  tcat.SetString(TABLENAME, tablename);
  tcat.SetInt(SLOTSIZE, layout.SlotSize());
  tcat.Close();

  /// There is one record in FLDCAT for each field of each table
  /// insert a record into FLDCAT for each field
  TableScan fcat = TableScan(tx, FIELDCAT, _fcatLayout);
  for (const std::string &fieldname : sch.Fields()) {
    fcat.Insert();
    fcat.SetString(TABLENAME, tablename);
    fcat.SetString("fldname", fieldname);
    fcat.SetInt("type", sch.Type(fieldname));
    fcat.SetInt("length", sch.Length(fieldname));
    fcat.SetInt("offset", layout.Offset(fieldname));
  }
  fcat.Close();
}

Layout TableManager::GetLayout(const std::string &tablename, Transaction *tx) {
  /// Uses table scans to get the records corresponding to the specified table
  /// name
  int size = -1;
  TableScan tcat = TableScan(tx, TABLECAT, _tcatLayout);
  while (tcat.Next()) {
    if (tcat.GetString(TABLENAME) == tablename) {
      size = tcat.GetInt(SLOTSIZE);
      break;
    }
  }

  if (size < 0) {
    std::string error_msg =
        "Table " + tablename + " does not exist in the database";
    throw std::runtime_error(error_msg);
  }

  tcat.Close();

  Schema sch;
  std::map<std::string, int> offsets;
  TableScan fcat = TableScan(tx, FIELDCAT, _fcatLayout);
  while (fcat.Next()) {
    if (fcat.GetString(TABLENAME) == tablename) {
      std::string fldname = fcat.GetString("fldname");
      int fldtype = fcat.GetInt("type");
      int fldlen = fcat.GetInt("length");
      int offset = fcat.GetInt("offset");
      offsets[fldname] = offset;
      sch.AddField(fldname, fldtype, fldlen);
    }
  }

  fcat.Close();

  return Layout(sch, offsets, size);
}
} // namespace simpledb
