#pragma once

#include "query/constant.hpp"
namespace simpledb {
class DirEntry {
public:
  DirEntry();
  DirEntry(const DirEntry &dirEntry);
  DirEntry(const Constant &dataVal, int blockNum);
  DirEntry &operator=(const DirEntry &dirEntry);
  Constant DataVal() const;
  int BlockNumber() const;
  bool IsNull() const;

private:
  Constant _data_val;
  int _block_num;
};
} // namespace simpledb
