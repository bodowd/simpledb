#include "materialize/sortplan.hpp"
#include "materialize/materializeplan.hpp"
#include "materialize/sortscan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

SortPlan::SortPlan(Transaction *tx, const std::shared_ptr<Plan> &plan,
                   const std::vector<std::string> &sortFields)
    : _plan(plan), _tx(tx), _sch(_plan->GetSchema()), _comp(sortFields) {}

std::shared_ptr<Scan> SortPlan::Open() {
  std::shared_ptr<Scan> src = _plan->Open();
  std::vector<std::shared_ptr<TempTable>> runs = splitIntoRuns(src.get());
  src->Close();
  while (runs.size() > 2) {
    runs = doAMergeIteration(runs);
  }
  return std::static_pointer_cast<Scan>(
      std::make_shared<SortScan>(runs, _comp));
}

int SortPlan::BlocksAccessed() {
  /// does not include the one-time cost of sorting
  auto plan = std::static_pointer_cast<Plan>(
      std::make_shared<MaterializePlan>(_tx, _plan));
  return plan->BlocksAccessed();
}

int SortPlan::RecordsOutput() { return _plan->RecordsOutput(); }

int SortPlan::DistinctValues(const std::string &fieldName) {
  return _plan->DistinctValues(fieldName);
}

std::vector<std::shared_ptr<TempTable>> SortPlan::splitIntoRuns(Scan *src) {
  std::vector<std::shared_ptr<TempTable>> temps;
  src->BeforeFirst();
  if (!src->Next()) {
    return temps;
  }

  auto currentTemp = std::make_shared<TempTable>(_tx, _sch);
  temps.emplace_back(currentTemp);
  std::shared_ptr<UpdateScan> currentScan = currentTemp->Open();
  while (copy(src, currentScan.get())) {
    if (_comp.Compare(src, currentScan.get()) < 0) {
      /// start a new run
      currentScan->Close();
      currentTemp = std::make_shared<TempTable>(_tx, _sch);
      temps.emplace_back(currentTemp);
      currentScan = std::static_pointer_cast<UpdateScan>(currentTemp->Open());
    }
  }

  currentScan->Close();
  return temps;
}

std::vector<std::shared_ptr<TempTable>> SortPlan::doAMergeIteration(
    const std::vector<std::shared_ptr<TempTable>> &runs) {
  std::vector<std::shared_ptr<TempTable>> result;

  for (int i = 0; i + 2 <= runs.size(); i += 2) {
    auto p1 = runs[i];
    auto p2 = runs[i + 1];
    result.emplace_back(mergeTwoRuns(p1.get(), p2.get()));
  }
  if (runs.size() % 2 == 1) {
    result.emplace_back(runs.back());
  }
  return result;
}

std::shared_ptr<TempTable> SortPlan::mergeTwoRuns(TempTable *p1,
                                                  TempTable *p2) {
  std::shared_ptr<Scan> src1 = p1->Open();
  std::shared_ptr<Scan> src2 = p2->Open();

  auto result = std::make_shared<TempTable>(_tx, _sch);
  std::shared_ptr<UpdateScan> dest = result->Open();

  bool hasMore1 = src1->Next();
  bool hasMore2 = src2->Next();
  /// This is where the two temp tables are merged into sorted order
  while (hasMore1 && hasMore2) {
    if (_comp.Compare(src1.get(), src2.get()) < 0) {
      hasMore1 = copy(src1.get(), dest.get());
    } else {
      hasMore2 = copy(src2.get(), dest.get());
    }
  }

  if (hasMore1) {
    while (hasMore1) {
      hasMore1 = copy(src1.get(), dest.get());
    }
  } else {
    while (hasMore2) {
      hasMore2 = copy(src2.get(), dest.get());
    }
  }

  src1->Close();
  src2->Close();
  dest->Close();
  return result;
}

bool SortPlan::copy(Scan *src, UpdateScan *dest) {
  dest->Insert();
  for (const std::string &fieldName : _sch.Fields()) {
    dest->SetVal(fieldName, src->GetVal(fieldName));
  }
  return src->Next();
}

} // namespace simpledb
