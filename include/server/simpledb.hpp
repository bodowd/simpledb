#include <file/filemanager.hpp>
#include <filesystem>
#include <memory>
#include <string>
namespace simpledb {
class SimpleDB {
public:
  int block_size;

  SimpleDB(std::string dir_name, int block_size, int buffer_size);
  std::unique_ptr<FileManager> GetFileManager();

private:
  std::unique_ptr<FileManager> _file_manager;
  int _block_size = 400;
};
} // namespace simpledb
