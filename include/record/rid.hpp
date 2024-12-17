#pragma once
#include <string>
namespace simpledb {

// p. 179 in the book
//
// Each record in a file can be identified by its record identifier (RID) which
// is composed of its block number in the file and its slot within the block.
class RID {
  friend bool operator==(const RID &lhs, const RID &rhs);

public:
  RID(const RID &rid);
  RID(int blknum, int slot);
  int BlockNumber() const;
  int Slot() const;
  bool Equals(const RID &rid) const;
  std::string ToString() const;

private:
  int _blknum;
  int _slot;
};
} // namespace simpledb
