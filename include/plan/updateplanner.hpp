#pragma once

#include "parse/createindexdata.hpp"
#include "parse/createtabledata.hpp"
#include "parse/createviewdata.hpp"
#include "parse/deletedata.hpp"
#include "parse/insertdata.hpp"
#include "parse/modifydata.hpp"
#include "tx/transaction.hpp"
namespace simpledb {
class UpdatePlanner {
public:
  virtual ~UpdatePlanner() {}
  virtual int ExecuteInsert(InsertData *id, Transaction *tx) = 0;
  virtual int ExecuteDelete(DeleteData *dd, Transaction *tx) = 0;
  virtual int ExecuteModify(ModifyData *md, Transaction *tx) = 0;
  virtual int ExecuteCreateTable(CreateTableData *ctd, Transaction *tx) = 0;
  virtual int ExecuteCreateView(CreateViewData *cvd, Transaction *tx) = 0;
  virtual int ExecuteCreateIndex(CreateIndexData *cid, Transaction *tx) = 0;
};

} // namespace simpledb
