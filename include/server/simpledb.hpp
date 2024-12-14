#pragma once
#include "log/logmanager.hpp"
#include "tx/transaction.hpp"
#include <buffer/buffermanager.hpp>
#include <file/filemanager.hpp>
#include <memory>
#include <string>
namespace simpledb {
class SimpleDB {
public:
  static int buffer_size;
  static int block_size;
  static std::string log_file;

  SimpleDB(std::string dir_name, int block_size, int buffer_size);
  // std::unique_ptr<FileManager> GetFileManager();
  FileManager &GetFileManager();
  LogManager &GetLogManager();
  // std::unique_ptr<LogManager> GetLogManager();
  // std::unique_ptr<BufferManager> GetBufferManager();
  BufferManager &GetBufferManager();
  std::unique_ptr<Transaction> NewTx();

private:
  std::unique_ptr<FileManager> _file_manager;
  std::unique_ptr<LogManager> _log_manager;
  std::unique_ptr<BufferManager> _buffer_manager;
};
} // namespace simpledb
