#pragma once

#include "metadata/metadatamanager.hpp"
#include "parse/createindexdata.hpp"
#include "parse/createtabledata.hpp"
#include "parse/createviewdata.hpp"
#include "plan/updateplanner.hpp"
#include "tx/transaction.hpp"
namespace simpledb {

class IndexUpdatePlanner : public UpdatePlanner {
public:
  IndexUpdatePlanner(MetadataManager *metadataManager);
  int ExecuteInsert(InsertData *insertData, Transaction *tx) override;
  int ExecuteDelete(DeleteData *deleteData, Transaction *tx) override;
  int ExecuteModify(ModifyData *modifyData, Transaction *tx) override;
  int ExecuteCreateTable(CreateTableData *createTableData,
                         Transaction *tx) override;
  int ExecuteCreateView(CreateViewData *createViewData,
                        Transaction *tx) override;
  int ExecuteCreateIndex(CreateIndexData *createIndexData,
                         Transaction *tx) override;

private:
  MetadataManager *_metadata_manager;
};
} // namespace simpledb
