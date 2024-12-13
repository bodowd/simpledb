#include "record/layout.hpp"
#include "tx/transaction.hpp"
#include <cmath>

namespace simpledb {
// RecordPage manages the records within a page.
// Implements a slotted page structure
// pg. 165 Fig. 6.5 in the book shows a diagram
// Here is an illustration
// [ slot 0] [ slot 1 ] [ slot 2 ]...
// [1, R0  ] [ 0, R1  ] [ 0, R2  ]...
// 01     27   28    54  55     81 ... 400
//
class RecordPage {
public:
  const int EMPTY = 0;
  const int USED = -1;
  RecordPage(Transaction *tx, const BlockId &blk, const Layout &layout);

  // Get the value of a specified field in the specified record
  int GetInt(int slot, const std::string &fieldname);
  std::string GetString(int slot, const std::string &fieldname);
  // Set the value of a specified field in the specified record
  void SetInt(int slot, const std::string &fieldname, int val);
  void SetString(int slot, const std::string &fieldname,
                 const std::string &val);

  // Set the record's flag to EMPTY
  void Delete(int slot);
  // Gives default values to all record slots in the page
  void Format();
  // Returns the first used slot that follows the specified slot
  // Skips any empty slots
  // A negative return value indicates all remaining slots are empty
  int NextAfter(int slot);
  // Returns the first empty slot following the specified slot
  // If an empty slot is found, the method sets its flag to USED and returns the
  // slot number. If no empty slot is found, -1 is returned
  int InsertAfter(int slot);
  BlockId Block() const;

private:
  Transaction *_tx;
  BlockId _blk;
  Layout _layout;

  void setFlag(int slot, int flag);
  // Repeatedly increment the specified slot until it finds a slot that has the
  // specified flag or it runs out of slots
  int searchAfter(int slot, int flag);
  bool isValidSlot(int slot);
  int offset(int slot) const;
};
} // namespace simpledb
