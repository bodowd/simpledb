#pragma once
#include <filesystem>

namespace simpledb {
// A BlockId identifies a specific block by its filename and logical block
// number.
//
// simpledb virtualizes the disk by storing the data in multiple OS files but
// treats the files as if they were raw disks. The db accesses the "disk" using
// logical file blocks. Page 65 in the book
class BlockId {
public:
  BlockId();

  BlockId(std::string filename, int block_num);

  std::string Filename() const;

  // return the block number
  int Number() const;

  std::string ToString();

  friend bool operator==(const BlockId &left, const BlockId &right);

private:
  std::string _filename;
  int _block_num;
};
} // namespace simpledb
