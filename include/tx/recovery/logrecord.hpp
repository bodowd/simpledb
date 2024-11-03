#include "file/blockid.hpp"
#include "log/logmanager.hpp"
#include <vector>

namespace simpledb {
class Transaction;
class LogRecord {
public:
  enum Operation {
    checkpoint = 0,
    start = 1,
    commit = 2,
    rollback = 3,
    setint = 4,
    setstring = 5
  };
  virtual ~LogRecord();
  virtual int Op() = 0;
  virtual int TxNumber() = 0;
  virtual std::string ToString() = 0;
  virtual void Undo(Transaction *tx) = 0;
  static std::unique_ptr<LogRecord>;
  std::unique_ptr<LogRecord> CreateLogRecord(const std::vector<char> &byteVec);
};

class CheckpointRecord : public LogRecord {
public:
  CheckpointRecord();
  int Op() override;
  int TxNumber() override;
  std::string ToString() override;
  void Undo(Transaction *tx) override;
  static int WriteToLog(LogManager *lm);
};

class StartRecord : public LogRecord {
public:
  StartRecord(Page *p);
  int Op() override;
  int TxNumber() override;
  std::string ToString() override;
  void Undo(Transaction *tx) override;
  static int WriteToLog(LogManager *lm, int txNum);

private:
  int _tx_num;
};

class CommitRecord : public LogRecord {
public:
  CommitRecord(Page *p);
  int Op() override;
  int TxNumber() override;
  std::string ToString() override;
  void Undo(Transaction *tx) override;
  static int WriteToLog(LogManager *lm, int txNum);

private:
  int _tx_num;
};

class RollbackRecord : public LogRecord {
public:
  RollbackRecord(Page *p);
  int Op() override;
  int TxNumber() override;
  std::string ToString() override;
  void Undo(Transaction *tx) override;
  static int WriteToLog(LogManager *lm, int txNum);

private:
  int _tx_num;
};

class SetIntRecord : public LogRecord {
public:
  SetIntRecord(Page *p);
  int Op() override;
  int TxNumber() override;
  std::string ToString() override;
  void Undo(Transaction *tx) override;
  static int WriteToLog(LogManager *lm, int txNum, const BlockId &blockId,
                        int offset, int val);

private:
  int _tx_num, _offset;
  int _val;
  BlockId _block_id;
};

class SetStringRecord : public LogRecord {
public:
  SetStringRecord(Page *p);
  int Op() override;
  int TxNumber() override;
  std::string ToString() override;
  void Undo(Transaction *tx) override;
  static int WriteToLog(LogManager *lm, int txNum, const BlockId &blockId,
                        int offset, const std::string &val);

private:
  int _tx_num, _offset;
  std::string _val;
  BlockId _block_id;
};
} // namespace simpledb
