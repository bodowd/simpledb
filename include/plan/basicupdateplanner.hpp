#pragma once

#include "metadata/metadatamanager.hpp"
#include "parse/createtabledata.hpp"
#include "parse/createviewdata.hpp"
#include "parse/modifydata.hpp"
#include "plan/updateplanner.hpp"
#include "tx/transaction.hpp"
namespace simpledb {
class BasicUpdatePlanner : public UpdatePlanner {
public:
  BasicUpdatePlanner(MetadataManager *metadatamanager);
  int ExecuteDelete(DeleteData *deletedata, Transaction *tx) override;
  int ExecuteModify(ModifyData *modifydata, Transaction *tx) override;
  int ExecuteInsert(InsertData *insertdata, Transaction *tx) override;
  int ExecuteCreateTable(CreateTableData *createtabledata,
                         Transaction *tx) override;
  int ExecuteCreateView(CreateViewData *createviewdata,
                        Transaction *tx) override;
  int ExecuteCreateIndex(CreateIndexData *createindexdata,
                         Transaction *tx) override;

private:
  MetadataManager *_metadata_manager;
};
} // namespace simpledb
