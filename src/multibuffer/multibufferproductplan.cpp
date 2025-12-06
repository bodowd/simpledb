#include "multibuffer/multibufferproductplan.hpp"
#include "materialize/materializeplan.hpp"
#include "multibuffer/multibufferproductscan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

MultiBufferProductPlan::MultiBufferProductPlan(
    Transaction &tx, const std::shared_ptr<Plan> &leftHandSide,
    const std::shared_ptr<Plan> &rightHandSide)
    : _tx(tx) {
  _left_hand_side = std::make_shared<MaterializePlan>(&tx, _left_hand_side);
  _right_hand_side = rightHandSide;
  _schema.AddAll(_left_hand_side->GetSchema());
  _schema.AddAll(_right_hand_side->GetSchema());
};

std::shared_ptr<Scan> MultiBufferProductPlan::Open() {
  std::shared_ptr<Scan> leftScan = _left_hand_side->Open();
  std::shared_ptr<TempTable> tempTable = copyRecordsFrom(*_right_hand_side);
  return std::static_pointer_cast<Scan>(
      std::make_shared<MultiBufferProductScan>(
          &_tx, leftScan, tempTable->TableName(), tempTable->GetLayout()));
}

int MultiBufferProductPlan::BlocksAccessed() {
  int avail = _tx.AvailableBuffs();
  int size = std::make_unique<MaterializePlan>(&_tx, _right_hand_side)
                 ->BlocksAccessed();
  int numChunks = size / avail;
  return _right_hand_side->BlocksAccessed() +
         (_left_hand_side->BlocksAccessed() * numChunks);
}

int MultiBufferProductPlan::RecordsOutput() {
  return _left_hand_side->RecordsOutput() * _right_hand_side->RecordsOutput();
}

int MultiBufferProductPlan::DistinctValues(const std::string &fieldName) {
  if (_left_hand_side->GetSchema().HasField(fieldName)) {
    return _left_hand_side->DistinctValues(fieldName);
  } else {
    return _right_hand_side->DistinctValues(fieldName);
  }
}

Schema MultiBufferProductPlan::GetSchema() { return _schema; }

std::shared_ptr<TempTable> MultiBufferProductPlan::copyRecordsFrom(Plan &plan) {
  std::shared_ptr<Scan> src = plan.Open();
  Schema sch = plan.GetSchema();
  auto t = std::make_shared<TempTable>(&_tx, sch);
  auto dest = std::static_pointer_cast<UpdateScan>(t->Open());
  while (src->Next()) {
    dest->Insert();
    for (const std::string &fieldName : sch.Fields()) {
      dest->SetVal(fieldName, src->GetVal(fieldName));
    }
  }
  src->Close();
  dest->Close();
  return t;
}
} // namespace simpledb
