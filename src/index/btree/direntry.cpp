#include "index/btree/direntry.hpp"

namespace simpledb {

DirEntry::DirEntry() {}

DirEntry::DirEntry(const DirEntry &dirEntry)
    : _data_val(dirEntry._data_val), _block_num(dirEntry._block_num) {}

DirEntry::DirEntry(const Constant &dataVal, int blockNum)
    : _data_val(dataVal), _block_num(blockNum) {}

DirEntry &DirEntry::operator=(const DirEntry &dirEntry) {
  if (this != &dirEntry) {
    _data_val = dirEntry._data_val;
    _block_num = dirEntry._block_num;
  }
  return *this;
}

Constant DirEntry::DataVal() const { return _data_val; }

int DirEntry::BlockNumber() const { return _block_num; }

bool DirEntry::IsNull() const { return _data_val.IsNull(); }
} // namespace simpledb
