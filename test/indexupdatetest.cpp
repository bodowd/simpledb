#include "metadata/metadatamanager.hpp"
#include "plan/tableplan.hpp"
#include "query/updatescan.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <memory>

namespace simpledb {
TEST(index, indexupdate_test) {
  /// IMPORTANT: this test requires a student table to exist which exists inside
  /// the `students` database. This database can be created by running
  /// ./build/bin/createstudentdb
  SimpleDB db("students");

  auto tx = db.NewTx();
  MetadataManager &mm = db.GetMetadataManager();

  std::shared_ptr<Plan> studentPlan = std::static_pointer_cast<Plan>(
      std::make_shared<TablePlan>(tx.get(), "student", &mm));
  std::shared_ptr<UpdateScan> studentScan =
      std::static_pointer_cast<UpdateScan>(studentPlan->Open());

  std::map<std::string, std::shared_ptr<Index>> indexes;
  std::map<std::string, IndexInfo> indexInfo =
      mm.GetIndexInfo("student", tx.get());
  for (const auto &[key, value] : indexInfo) {
    std::cout << key << "\n";
  }
  for (const auto &[fieldname, ii] : indexInfo) {
    indexes[fieldname] = indexInfo.at(fieldname).Open();
  }

  studentScan->Insert();
  studentScan->SetInt("sid", 1);
  studentScan->SetString("sname", "sam");
  studentScan->SetInt("gradyear", 2023);
  studentScan->SetInt("majorid", 30);

  RID dataRID = studentScan->GetRid();
  for (const auto &[fieldname, idx] : indexes) {
    Constant dataVal = studentScan->GetVal(fieldname);
    idx->Insert(dataVal, dataRID);
  }

  studentScan->BeforeFirst();
  while (studentScan->Next()) {
    if (studentScan->GetString("sname") == "joe") {
      RID joeRID = studentScan->GetRid();
      for (const auto &[fieldname, idx] : indexes) {
        Constant dataval = studentScan->GetVal(fieldname);
        idx->Remove(dataval, joeRID);
      }
      studentScan->Delete();
      break;
    }
  }

  studentScan->BeforeFirst();
  while (studentScan->Next()) {
    std::cout << studentScan->GetString("sname") << " "
              << studentScan->GetInt("sid") << std::endl;
  }

  studentScan->Close();
  for (const auto &[fieldname, idx] : indexes) {
    idx->Close();
  }
  tx->Commit();
}
} // namespace simpledb
