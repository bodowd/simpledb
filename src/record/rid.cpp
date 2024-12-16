#include "record/rid.hpp"
#include <string>
namespace simpledb {

bool operator==(const RID &lhs, const RID &rhs) {
  return lhs._blknum == rhs._blknum && lhs._slot == rhs._slot;
}

RID::RID(const RID &rid) {
  _blknum = rid._blknum;
  _slot = rid._slot;
}

RID::RID(int blknum, int slot) {
  _blknum = blknum;
  _slot = slot;
}

int RID::BlockNumber() const { return _blknum; }

int RID::Slot() const { return _slot; }

bool RID::Equals(const RID &rid) const {
  return _blknum == rid.BlockNumber() && _slot == rid.Slot();
}

std::string RID::ToString() const {
  return "[" + std::to_string(_blknum) + ", " + std::to_string(_slot) + "]";
}

} // namespace simpledb
