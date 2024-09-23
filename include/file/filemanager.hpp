#include "blockid.hpp"
#include "page.hpp"
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>

namespace simpledb {
class FileManager {
  // The FileManager handles the actual interaction with the OS file system
public:
  FileManager(const std::filesystem::path &db_directory, int block_size);
  void Read(BlockId &blk, Page &p);
  void Write(BlockId &blk, Page &p);
  BlockId Append(std::string filename);
  bool IsNew();
  int Length(const std::string filename);
  int BlockSize();

private:
  std::shared_ptr<std::fstream> getFile(const std::string &filename);
  std::filesystem::path _db_directory;
  int _block_size;
  bool _is_new;
  std::map<std::filesystem::path, std::shared_ptr<std::fstream>> _open_files;
  std::mutex _mutex;
};
} // namespace simpledb
