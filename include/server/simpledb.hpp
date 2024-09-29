#pragma once
#include "log/logmanager.hpp"
#include <file/filemanager.hpp>
#include <memory>
#include <string>
namespace simpledb {
class SimpleDB {
public:
  static int block_size;
  static std::string log_file;

  SimpleDB(std::string dir_name, int block_size, int buffer_size);
  std::unique_ptr<FileManager> GetFileManager();
  std::unique_ptr<LogManager> GetLogManager();

private:
  std::unique_ptr<FileManager> _file_manager;
  std::unique_ptr<LogManager> _log_manager;
};
} // namespace simpledb
