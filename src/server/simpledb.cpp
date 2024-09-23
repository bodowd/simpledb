#include "server/simpledb.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace simpledb {

SimpleDB::SimpleDB(const std::string dir_name, int block_size,
                   int buffer_size) {
  auto path = std::filesystem::current_path() / dir_name;
  _file_manager = std::make_unique<FileManager>(path, block_size);
};

std::unique_ptr<FileManager> SimpleDB::GetFileManager() {
  return std::move(_file_manager);
}

} // namespace simpledb
