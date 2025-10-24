#include "plan/basicupdateplanner.hpp"
#include "metadata/metadatamanager.hpp"
#include "parse/createindexdata.hpp"
#include "parse/createtabledata.hpp"
#include "parse/modifydata.hpp"
#include "plan/selectplan.hpp"
#include "plan/tableplan.hpp"
#include "query/constant.hpp"
#include "query/updatescan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {
BasicUpdatePlanner::BasicUpdatePlanner(MetadataManager *metadatamanager)
    : _metadata_manager(metadatamanager) {}

int BasicUpdatePlanner::ExecuteDelete(DeleteData *deletedata, Transaction *tx) {
  /// The TablePlan will be passed to the SelectPlan's constructor making
  /// TablePlan the child of SelectPlan. This forms a tree hierarchy
  std::shared_ptr<Plan> plan =
      std::static_pointer_cast<Plan>(std::make_shared<TablePlan>(
          tx, deletedata->TableName(), _metadata_manager));
  plan = std::static_pointer_cast<Plan>(
      std::make_shared<SelectPlan>(plan, deletedata->Pred()));
  std::shared_ptr<UpdateScan> updatescan =
      std::static_pointer_cast<UpdateScan>(plan->Open());
  int count = 0;
  while (updatescan->Next()) {
    updatescan->Delete();
    count++;
  }
  updatescan->Close();
  return count;
}

int BasicUpdatePlanner::ExecuteModify(ModifyData *modifydata, Transaction *tx) {
  std::shared_ptr<Plan> plan =
      std::static_pointer_cast<Plan>(std::make_shared<TablePlan>(
          tx, modifydata->TableName(), _metadata_manager));
  plan = std::static_pointer_cast<Plan>(
      std::make_shared<SelectPlan>(plan, modifydata->Pred()));
  std::shared_ptr<UpdateScan> updatescan =
      std::static_pointer_cast<UpdateScan>(plan->Open());
  int count = 0;
  while (updatescan->Next()) {
    Constant val = modifydata->NewVal().Evaluate(updatescan.get());
    updatescan->SetVal(modifydata->TargetField(), val);
    count++;
  }
  updatescan->Close();
  return count;
}

int BasicUpdatePlanner::ExecuteInsert(InsertData *insertdata, Transaction *tx) {
  std::shared_ptr<Plan> plan =
      std::static_pointer_cast<Plan>(std::make_shared<TablePlan>(
          tx, insertdata->TableName(), _metadata_manager));
  std::shared_ptr<UpdateScan> updatescan =
      std::static_pointer_cast<UpdateScan>(plan->Open());
  updatescan->Insert();
  std::vector<Constant> vals = insertdata->Vals();
  std::vector<std::string> fields = insertdata->Fields();
  for (int i = 0; i < static_cast<int>(vals.size()); i++) {
    updatescan->SetVal(fields[i], vals[i]);
  }
  updatescan->Close();
  return 1;
}

int BasicUpdatePlanner::ExecuteCreateTable(CreateTableData *createtabledata,
                                           Transaction *tx) {
  _metadata_manager->CreateTable(createtabledata->TableName(),
                                 createtabledata->NewSchema(), tx);
  return 0;
}

int BasicUpdatePlanner::ExecuteCreateView(CreateViewData *createviewdata,
                                          Transaction *tx) {
  _metadata_manager->CreateView(createviewdata->ViewName(),
                                createviewdata->ViewDef(), tx);
  return 0;
}

int BasicUpdatePlanner::ExecuteCreateIndex(CreateIndexData *createindexdata,
                                           Transaction *tx) {
  _metadata_manager->CreateIndex(
      createindexdata->IndexName(), createindexdata->TableName(),
      createindexdata->FieldName(), createindexdata->IndexType(), tx);
  return 0;
}
} // namespace simpledb
