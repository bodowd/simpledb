#include <memory>
#include <vector>
namespace simpledb {

// A Page object holds the contents of a disk block.
// It is a block-sized area of memory
class Page {
public:
  Page(int block_size);
  Page(const std::shared_ptr<std::vector<char>> &byte_buffer);

  int GetInt(int offset);
  std::vector<char> GetBytes(int offset);
  std::string GetString(int offset);
  void SetInt(int offset, int val);
  void SetBytes(int offset, const std::vector<char> &byte_buffer);
  void SetString(int offset, std::string val);
  int MaxLength(int strlen);

  std::shared_ptr<std::vector<char>>
  contents(); // sometimes page contents should be alive after page is destroyed

private:
  std::shared_ptr<std::vector<char>> _byte_buffer;
};
} // namespace simpledb
