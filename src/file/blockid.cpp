#include "file/blockid.hpp"
#include <string>

namespace simpledb {
BlockId::BlockId() {};

BlockId::BlockId(std::string filename, int block_num)
    : _filename(filename), _block_num(block_num) {};

std::string BlockId::Filename() { return _filename; }

int BlockId::Number() { return _block_num; }

bool operator==(const BlockId &left, const BlockId &right) {
  return left._filename == right._filename &&
         left._block_num == right._block_num;
}

std::string BlockId::ToString() {
  return _filename + ",  " + std::to_string(_block_num);
}

} // namespace simpledb
