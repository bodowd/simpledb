#include "tx/recovery/logrecord.hpp"
#include "file/page.hpp"
#include "log/logmanager.hpp"
#include <memory>
#include <string>
#include <vector>

namespace simpledb {
std::unique_ptr<LogRecord>
LogRecord::CreateLogRecord(const std::vector<char> &byteVec) {
  auto _byteVec = std::make_shared<std::vector<char>>(byteVec);
  Page p = Page(_byteVec);
  Operation op = static_cast<Operation>(p.GetInt(0));
  switch (op) {
  case checkpoint:
    return std::make_unique<CheckpointRecord>();
  case start:
    return std::make_unique<StartRecord>(&p);
  case commit:
    return std::make_unique<CommitRecord>(&p);
  case rollback:
    return std::make_unique<RollbackRecord>(&p);
  case setint:
    return std::make_unique<SetIntRecord>(&p);
  case setstring:
    return std::make_unique<SetStringRecord>(&p);
  default:
    return std::unique_ptr<LogRecord>(nullptr);
  }
}

CheckpointRecord::CheckpointRecord() {
  // do nothing;
}

int CheckpointRecord::Op() { return checkpoint; }

int CheckpointRecord::TxNumber() { return -1; }

std::string CheckpointRecord::ToString() { return "<CHECKPOINT>"; }

void CheckpointRecord::Undo(Transaction *tx) {
  // do nothing;
}

int CheckpointRecord::WriteToLog(LogManager *lm) {
  auto byteVec = std::make_shared<std::vector<char>>(sizeof(int), 0);
  Page p(byteVec);
  p.SetInt(0, checkpoint);
  return lm->Append(*byteVec);
}

// Read the record from the page and populate the private variables
StartRecord::StartRecord(Page *p) {
  int tx_num_pos = sizeof(int);
  _tx_num = p->GetInt(tx_num_pos);
}

int StartRecord::Op() { return start; }

int StartRecord::TxNumber() { return _tx_num; }

std::string StartRecord::ToString() {
  return "<START, " + std::to_string(_tx_num) + ">";
}

void StartRecord::Undo(Transaction *tx) {
  // do nothing;
}

int StartRecord::WriteToLog(LogManager *lm, int txNum) {
  auto tx_num_pos = sizeof(int);
  auto record_len = tx_num_pos + sizeof(int);
  auto byteVec = std::make_shared<std::vector<char>>(record_len, 0);
  Page p(byteVec);
  p.SetInt(0, start);
  p.SetInt(tx_num_pos, txNum);
  return lm->Append(*byteVec);
}

// Read the record from the page and populate the private variables
CommitRecord::CommitRecord(Page *p) {
  int tx_num_pos = sizeof(int);
  _tx_num = p->GetInt(tx_num_pos);
}

int CommitRecord::Op() { return commit; }

int CommitRecord::TxNumber() { return _tx_num; }

std::string CommitRecord::ToString() {
  return "<COMMIT, " + std::to_string(_tx_num) + ">";
}

void CommitRecord::Undo(Transaction *tx) {
  // do nothing;
}

int CommitRecord::WriteToLog(LogManager *lm, int txNum) {
  int tx_num_pos = sizeof(int);
  int record_len = tx_num_pos + sizeof(int);
  auto byteVec = std::make_shared<std::vector<char>>(record_len, 0);
  Page p(byteVec);
  p.SetInt(0, commit);
  p.SetInt(tx_num_pos, txNum);
  return lm->Append(*byteVec);
}

// Read the record from the page and populate the private variables
RollbackRecord::RollbackRecord(Page *p) {
  int tx_num_pos = sizeof(int);
  _tx_num = p->GetInt(tx_num_pos);
}

int RollbackRecord::Op() { return rollback; }

int RollbackRecord::TxNumber() { return _tx_num; }

std::string RollbackRecord::ToString() {
  return "<ROLLBACK, " + std::to_string(_tx_num) + ">";
}

void RollbackRecord::Undo(Transaction *tx) {
  // do nothing;
}

int RollbackRecord::WriteToLog(LogManager *lm, int txNum) {
  int tx_num_pos = sizeof(int);
  int record_len = tx_num_pos + sizeof(int);
  auto byteVec = std::make_shared<std::vector<char>>(record_len, 0);
  Page p(byteVec);
  p.SetInt(0, rollback);
  p.SetInt(tx_num_pos, txNum);
  return lm->Append(*byteVec);
}

// Read the record from the page and populate the private variables
SetIntRecord::SetIntRecord(Page *p) {
  int tx_num_pos = sizeof(int);
  _tx_num = p->GetInt(tx_num_pos);

  int file_pos = tx_num_pos + sizeof(int);
  std::string file_name = p->GetString(file_pos);
  int block_id_pos = file_pos + p->MaxLength(file_name.size());
  int block_num = p->GetInt(block_id_pos);
  _block_id = BlockId(file_name, block_num);

  int offset_pos = block_id_pos + sizeof(int);
  _offset = p->GetInt(offset_pos);

  int val_pos = offset_pos + sizeof(int);
  _val = p->GetInt(val_pos);
}

int SetIntRecord::Op() { return setint; }

int SetIntRecord::TxNumber() { return _tx_num; }

std::string SetIntRecord::ToString() {
  return "<SETINT, " + std::to_string(_tx_num) + ", " + _block_id.ToString() +
         ", " + std::to_string(_offset) + ", " + std::to_string(_val) + ">";
}

void SetIntRecord::Undo(Transaction *tx) {
  // TODO: uncomment after implementing the transaction class
  // tx->Pin(_block_id);
  // tx->SetInt(_block_id, _offset, _val, false)
  // tx->Unpin(_block_id)
}

int SetIntRecord::WriteToLog(LogManager *lm, int txNum, const BlockId &blockId,
                             int offset, int val) {
  int tx_num_pos = sizeof(int);
  int file_pos = tx_num_pos + sizeof(int);
  int block_id_pos = file_pos + Page::MaxLength(blockId.Filename().size());
  int offset_pos = block_id_pos + sizeof(int);
  int val_pos = offset_pos + sizeof(int);
  int record_len = val_pos + sizeof(int);
  auto byteVec = std::make_shared<std::vector<char>>(record_len, 0);
  Page p(byteVec);
  p.SetInt(0, setint);
  p.SetInt(tx_num_pos, txNum);
  p.SetString(file_pos, blockId.Filename());
  p.SetInt(block_id_pos, blockId.Number());
  p.SetInt(offset_pos, offset);
  p.SetInt(val_pos, val);
  return lm->Append(*byteVec);
}

SetStringRecord::SetStringRecord(Page *p) {
  int tx_num_pos = sizeof(int);
  _tx_num = p->GetInt(tx_num_pos);

  int file_pos = tx_num_pos + sizeof(int);
  std::string file_name = p->GetString(file_pos);
  int block_id_pos = file_pos + p->MaxLength(file_name.size());
  int block_num = p->GetInt(block_id_pos);
  _block_id = BlockId(file_name, block_num);

  int offset_pos = block_id_pos + sizeof(int);
  _offset = p->GetInt(offset_pos);

  int val_pos = offset_pos + sizeof(int);
  _val = p->GetInt(val_pos);
}

int SetStringRecord::Op() { return setstring; }

int SetStringRecord::TxNumber() { return _tx_num; }

std::string SetStringRecord::ToString() {
  return "<SETSTRING, " + std::to_string(_tx_num) + ", " +
         _block_id.ToString() + ", " + std::to_string(_offset) + ", " + _val +
         ">";
}

void SetStringRecord::Undo(Transaction *tx) {
  // TODO: uncomment after implementing the transaction class
  // tx->Pin(_block_id);
  // tx->SetString(_block_id, _offset, _val, false);
  // tx->UnPin(_block_id);
}

int SetStringRecord::WriteToLog(LogManager *lm, int txNum,
                                const BlockId &blockId, int offset,
                                const std::string &val) {
  int tx_num_pos = sizeof(int);
  int file_pos = tx_num_pos + sizeof(int);
  int block_id_pos = file_pos + Page::MaxLength(blockId.Filename().size());
  int offset_pos = block_id_pos + sizeof(int);
  int val_pos = offset_pos + sizeof(int);
  int record_len = val_pos + Page::MaxLength(val.size());
  auto byteVec = std::make_shared<std::vector<char>>(record_len, 0);
  Page p(byteVec);
  p.SetInt(0, setstring);
  p.SetInt(tx_num_pos, txNum);
  p.SetString(file_pos, blockId.Filename());
  p.SetInt(block_id_pos, blockId.Number());
  p.SetInt(offset_pos, offset);
  p.SetString(val_pos, val);
  return lm->Append(*byteVec);
}

} // namespace simpledb
