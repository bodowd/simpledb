#include "file/blockid.hpp"
#include <string>

namespace simpledb {
bool operator==(const BlockId &left, const BlockId &right) {
  return left._filename == right._filename &&
         left._block_num == right._block_num;
}

bool operator!=(const BlockId &left, const BlockId &right) {
  if (left == right) {
    return false;
  } else {
    return true;
  }
}

bool operator<(const BlockId &left, const BlockId &right) {
  if (left._filename == right._filename) {
    return left._block_num < right._block_num;
  } else {
    return left._filename < right._filename;
  }
}

bool operator>(const BlockId &left, const BlockId &right) {
  if (left._filename == right._filename) {
    return left._block_num > right._block_num;
  } else {
    return left._filename > right._filename;
  }
}

bool operator<=(const BlockId &left, const BlockId &right) {
  if (left > right) {
    return false;
  } else {
    return true;
  }
}

bool operator>=(const BlockId &left, const BlockId &right) {
  if (left < right) {
    return false;
  } else {
    return true;
  }
}

BlockId &BlockId::operator=(const BlockId &obj) {
  if (this != &obj) {
    _filename = obj._filename;
    _block_num = obj._block_num;
  }
  return *this;
}

BlockId::BlockId() {};

BlockId::BlockId(std::string filename, int block_num)
    : _filename(filename), _block_num(block_num) {};

BlockId::BlockId(const BlockId &blkId) {
  _filename = blkId._filename;
  _block_num = blkId._block_num;
}

std::string BlockId::Filename() const { return _filename; }

int BlockId::Number() const { return _block_num; }

bool BlockId::IsNull() { return _filename.empty(); }

std::string BlockId::ToString() {
  return _filename + ",  " + std::to_string(_block_num);
}

} // namespace simpledb
