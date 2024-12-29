#pragma once
#include "query/constant.hpp"
#include "query/updatescan.hpp"
#include "record/recordpage.hpp"
#include "record/rid.hpp"
#include "tx/transaction.hpp"
namespace simpledb {

//
// p. 178 in the book
// Manages the records in a table. TableScan object keeps track of a current
// record and its methods change the current record and access its contents.
// The TableScan class provides a different level of abstraction from the Page,
// Buffer, Transaction, and RecordPage classes. Those methods all apply to a
// particular block. The TableScan class hides the block structure from its
// clients.
class TableScan : public UpdateScan {
public:
  TableScan(Transaction *tx, const std::string &table_name,
            const Layout &layout);
  ~TableScan();
  // Positions the current record before the first record of the file.
  void BeforeFirst() override;
  // Positions the current record at the next record in the file.
  //
  // If no more records, will read succeeding blocks in the file until another
  // record is found.
  //
  // If no more records can be found, returns false
  bool Next() override;
  // Applies to the current record
  int GetInt(const std::string &fieldname) override;
  // Applies to the current record
  std::string GetString(const std::string &fieldname) override;
  Constant GetVal(const std::string &fieldname) override;
  bool HasField(const std::string &fieldname) override;
  void Close() override;
  // Applies to the current record
  void SetInt(const std::string &fieldname, int val) override;
  // Applies to the current record
  void SetString(const std::string &fieldname, const std::string &val) override;
  void SetVal(const std::string &fieldname, const Constant &val) override;
  // Insert a new record somewhere in the file, starting with the current
  // record's block.
  //
  // Always succeeds.
  //
  // If it cannot find a place to insert the record in the existing blocks of
  // the file, appends a new block to the file and inserts the record there.
  void Insert() override;
  // Applies to the current record
  void Delete() override;
  // Positions the current record at the specified RID.
  void MoveToRid(const RID &rid) override;
  // Returns the RID of the current record
  RID GetRid() override;

private:
  Transaction *_tx;
  Layout _layout;
  std::unique_ptr<RecordPage> _rp;
  std::string _filename;
  int _currentSlot;

  void moveToBlock(int blknum);
  void moveToNewBlock();
  bool atLastBlock();
};
} // namespace simpledb
