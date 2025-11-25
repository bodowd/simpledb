#include "index/planner/indexupdateplanner.hpp"
#include "metadata/indexmanager.hpp"
#include "metadata/metadatamanager.hpp"
#include "parse/createtabledata.hpp"
#include "parse/deletedata.hpp"
#include "parse/insertdata.hpp"
#include "plan/selectplan.hpp"
#include "plan/tableplan.hpp"
#include "query/updatescan.hpp"
#include <memory>
#include <stdexcept>
namespace simpledb {
IndexUpdatePlanner::IndexUpdatePlanner(MetadataManager *metadataManager)
    : _metadata_manager(metadataManager) {}

int IndexUpdatePlanner::ExecuteInsert(InsertData *insertData, Transaction *tx) {
  std::string tableName = insertData->TableName();
  std::shared_ptr<Plan> p = std::static_pointer_cast<Plan>(
      std::make_shared<TablePlan>(tx, tableName, _metadata_manager));

  /// first insert the record
  std::shared_ptr<UpdateScan> scan =
      std::dynamic_pointer_cast<UpdateScan>(p->Open());
  if (!scan) {
    throw std::runtime_error(
        "Could not convert the scan to an update scan in ExecuteInsert");
  }
  scan->Insert();
  RID rid = scan->GetRid();

  /// now modify each field and insert index records
  std::map<std::string, IndexInfo> indexes =
      _metadata_manager->GetIndexInfo(tableName, tx);
  std::vector<std::string> fields = insertData->Fields();
  std::vector<Constant> vals = insertData->Vals();
  for (int i = 0; i < static_cast<int>(fields.size()); i++) {
    /// first modify the field that is due to the update
    std::string fieldname = fields[i];
    Constant val = vals[i];
    scan->SetVal(fieldname, val);

    /// if there is an index on this fieldname in this table, then update the
    /// index
    if (indexes.find(fieldname) != indexes.end()) {
      std::shared_ptr<Index> index = indexes[fieldname].Open();
      index->Insert(val, rid);
      index->Close();
    }
  }
  scan->Close();
  return 1;
}

int IndexUpdatePlanner::ExecuteDelete(DeleteData *deleteData, Transaction *tx) {
  std::string tableName = deleteData->TableName();
  std::shared_ptr<Plan> plan = std::static_pointer_cast<Plan>(
      std::make_shared<TablePlan>(tx, tableName, _metadata_manager));
  plan = std::static_pointer_cast<Plan>(
      std::make_shared<SelectPlan>(plan, deleteData->Pred()));
  std::map<std::string, IndexInfo> indexes =
      _metadata_manager->GetIndexInfo(tableName, tx);
  std::shared_ptr<UpdateScan> scan =
      std::dynamic_pointer_cast<UpdateScan>(plan->Open());
  if (!scan) {
    throw std::runtime_error(
        "Could not convert the scan to an update scan in ExecuteDelete");
  }
  int count = 0;
  while (scan->Next()) {
    /// delete the record's RID from every index in the table
    RID rid = scan->GetRid();
    for (const auto &[fieldName, iI] : indexes) {
      Constant val = scan->GetVal(fieldName);
      std::shared_ptr<Index> index = iI.Open();
      index->Remove(val, rid);
      index->Close();
    }
    /// now delete the record from the table via the scan of the table
    scan->Delete();
    count++;
  }
  scan->Close();
  return count;
}

int IndexUpdatePlanner::ExecuteModify(ModifyData *modifyData, Transaction *tx) {
  std::string tablename = modifyData->TableName();
  std::string fieldname = modifyData->TargetField();
  std::shared_ptr<Plan> plan = std::static_pointer_cast<Plan>(
      std::make_shared<TablePlan>(tx, tablename, _metadata_manager));
  plan = std::static_pointer_cast<Plan>(
      std::make_shared<SelectPlan>(plan, modifyData->Pred()));

  std::map<std::string, IndexInfo> indexes =
      _metadata_manager->GetIndexInfo(tablename, tx);
  std::shared_ptr<Index> index =
      (indexes.find(fieldname) == indexes.end() ? nullptr
                                                : indexes[fieldname].Open());
  std::shared_ptr<UpdateScan> scan =
      std::dynamic_pointer_cast<UpdateScan>(plan->Open());
  if (!scan) {
    throw std::runtime_error(
        "Could not convert the scan to an update scan in ExecuteModify");
  }
  int count = 0;
  while (scan->Next()) {
    /// first update the record
    Constant newVal = modifyData->NewVal().Evaluate(scan.get());
    Constant oldVal = scan->GetVal(fieldname);
    scan->SetVal(modifyData->TargetField(), newVal);

    /// update the index now if it exists
    if (index) {
      RID rid = scan->GetRid();
      index->Remove(oldVal, rid);
      index->Insert(newVal, rid);
    }
    count++;
  }
  if (index) {
    index->Close();
  }
  scan->Close();
  return count;
}

int IndexUpdatePlanner::ExecuteCreateTable(CreateTableData *createTableData,
                                           Transaction *tx) {
  _metadata_manager->CreateTable(createTableData->TableName(),
                                 createTableData->NewSchema(), tx);
  return 0;
}

int IndexUpdatePlanner::ExecuteCreateView(CreateViewData *createViewData,
                                          Transaction *tx) {
  _metadata_manager->CreateView(createViewData->ViewName(),
                                createViewData->ViewDef(), tx);
  return 0;
}

int IndexUpdatePlanner::ExecuteCreateIndex(CreateIndexData *createIndexData,
                                           Transaction *tx) {
  _metadata_manager->CreateIndex(
      createIndexData->IndexName(), createIndexData->TableName(),
      createIndexData->FieldName(), createIndexData->IndexType(), tx);
  return 0;
}

} // namespace simpledb
