#include "server/simpledb.hpp"
#include "log/logmanager.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace simpledb {

SimpleDB::SimpleDB(const std::string dir_name, int block_size,
                   int buffer_size) {
  auto path = std::filesystem::current_path() / dir_name;
  _file_manager = std::make_unique<FileManager>(path, block_size);
  _log_manager =
      std::make_unique<LogManager>(_file_manager.get(), "simpledb.log");
  _buffer_manager = std::make_unique<BufferManager>(
      _file_manager.get(), _log_manager.get(), buffer_size);
};

std::unique_ptr<FileManager> SimpleDB::GetFileManager() {
  return std::move(_file_manager);
}

std::unique_ptr<LogManager> SimpleDB::GetLogManager() {
  return std::move(_log_manager);
}

BufferManager &SimpleDB::GetBufferManager() { return *_buffer_manager; }

} // namespace simpledb
