#include "metadata/indexmanager.hpp"
#include "metadata/metadatamanager.hpp"
#include "plan/tableplan.hpp"
#include "query/updatescan.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <memory>

namespace simpledb {
TEST(index, indexretrieval_test) {
  /// IMPORTANT: this test requires a student table to exist which exists inside
  /// the `students` database. This database can be created by running
  /// ./build/bin/createstudentdb
  SimpleDB db("students");

  auto tx = db.NewTx();

  MetadataManager &metadataManager = db.GetMetadataManager();

  std::shared_ptr<Plan> studentPlan = std::static_pointer_cast<Plan>(
      std::make_shared<TablePlan>(tx.get(), "student", &metadataManager));

  std::shared_ptr<UpdateScan> studentScan =
      std::static_pointer_cast<UpdateScan>(studentPlan->Open());
  Layout lt = metadataManager.GetLayout("student", tx.get());

  std::map<std::string, IndexInfo> indexes =
      metadataManager.GetIndexInfo("student", tx.get());

  IndexInfo indexInfo = indexes.at("majorid");
  std::shared_ptr<Index> index = indexInfo.Open();

  index->BeforeFirst(Constant(20));
  while (index->Next()) {
    RID dataRID = index->GetDataRid();
    studentScan->MoveToRid(dataRID);
    std::cout << studentScan->GetString("sname") << std::endl;
  }

  index->Close();
  studentScan->Close();
  tx->Commit();
}
} // namespace simpledb
