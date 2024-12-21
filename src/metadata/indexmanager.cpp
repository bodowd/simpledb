#include "metadata/indexmanager.hpp"
#include "record/schema.hpp"
#include "record/tablescan.hpp"
#include "tx/transaction.hpp"
#include <iostream>
#include <memory>

namespace simpledb {

IndexInfo::IndexInfo() {}

IndexInfo::IndexInfo(const IndexInfo &ii)
    : _idxname(ii._idxname), _fieldname(ii._fieldname), _tx(ii._tx),
      _tblSchema(ii._tblSchema), _idxLayout(ii._idxLayout), _si(ii._si) {}

IndexInfo::IndexInfo(const std::string &idxname, const std::string &fieldname,
                     const Schema &tableSchema, Transaction *tx,
                     const StatInfo &si)
    : _idxname(idxname), _fieldname(fieldname), _tx(tx),
      _tblSchema(tableSchema), _si(si) {

  _idxLayout = createIdxLayout();
}

IndexInfo &IndexInfo::operator=(const IndexInfo &ii) {
  if (this != &ii) {
    _idxname = ii._idxname;
    _fieldname = ii._fieldname;
    _tx = ii._tx;
    _tblSchema = ii._tblSchema;
    _idxLayout = ii._idxLayout;
    _si = ii._si;
  }
  return *this;
}

std::shared_ptr<Index> IndexInfo::Open() {
  std::cout << "INDEX INFO OPEN" << std::endl;
  Schema sch;
  return std::static_pointer_cast<Index>(
      std::make_shared<Index>()); // temporary
}

int IndexInfo::BlocksAccessed() {
  int recordsPerBlock = _tx->BlockSize() / _idxLayout.SlotSize();
  // size of the index file
  int numBlocks = _si.RecordsOutput() / recordsPerBlock;
  return 1; // temporary
  // return HashIndex.searchCost(numBlocks, rpb);
}

int IndexInfo::RecordsOutput() {
  return _si.RecordsOutput() / _si.DistinctValues(_fieldname);
}

int IndexInfo::DistinctValues(const std::string &fieldname) {
  return _fieldname == fieldname ? 1 : _si.DistinctValues(fieldname);
}

Layout IndexInfo::createIdxLayout() {
  Schema sch;
  sch.AddIntField("block");
  sch.AddIntField("id");
  if (_tblSchema.Type(_fieldname) == Schema::INTEGER) {
    sch.AddIntField("dataval");
  } else {
    int fldlen = _tblSchema.Length(_fieldname);
    sch.AddStringField("dataval", fldlen);
  }
  return Layout(sch);
}

IndexManager::IndexManager(bool isNew, TableManager *tm, StatManager *sm,
                           Transaction *tx) {
  if (isNew) {
    Schema sch;
    sch.AddStringField("indexname", tm->MAX_NAME);
    sch.AddStringField("tablename", tm->MAX_NAME);
    sch.AddStringField("fieldname", tm->MAX_NAME);
    tm->CreateTable("idxcat", sch, tx);
  }

  _tm = tm;
  _sm = sm;
  _layout = _tm->GetLayout("idxcat", tx);
}

void IndexManager::CreateIndex(const std::string &idxname,
                               const std::string &tablename,
                               const std::string &fieldname, Transaction *tx) {
  TableScan ts(tx, "idxcat", _layout);
  ts.Insert();
  ts.SetString("indexname", idxname);
  ts.SetString("tablename", tablename);
  ts.SetString("fieldname", fieldname);
  ts.Close();
}

std::map<std::string, IndexInfo>
IndexManager::GetIndexInfo(const std::string &tablename,
                           Transaction *tx) const {
  std::map<std::string, IndexInfo> result;
  TableScan ts(tx, "idxcat", _layout);
  while (ts.Next()) {
    if (ts.GetString("tablename") == tablename) {
      std::string idxname = ts.GetString("indexname");
      std::string fieldname = ts.GetString("fieldname");
      Layout tblLayout = _tm->GetLayout(tablename, tx);
      StatInfo tblSi = _sm->GetStatInfo(tablename, tblLayout, tx);
      IndexInfo ii(idxname, fieldname, tblLayout.GetSchema(), tx, tblSi);
      result[fieldname] = ii;
    }
  }
  ts.Close();
  return result;
}

} // namespace simpledb
