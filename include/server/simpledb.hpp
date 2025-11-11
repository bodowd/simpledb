#pragma once
#include "log/logmanager.hpp"
#include "metadata/metadatamanager.hpp"
#include "plan/planner.hpp"
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

  /// Some tests require different settings and manager modules so there are two
  /// different constructors for convenience At the time the initial modules
  /// were developed, some managers did not yet exist. Later, these managers
  /// like metadata manager were developed, and these are used in the second
  /// constructor because they are needed for different tests on later modules
  /// that build upon former modules
  SimpleDB(const std::string dir_name, int block_size, int buffer_size);
  SimpleDB(const std::string &dir_name);
  FileManager &GetFileManager();
  LogManager &GetLogManager();
  BufferManager &GetBufferManager();
  MetadataManager &GetMetadataManager();
  std::unique_ptr<Transaction> NewTx();
  Planner &GetPlanner();

private:
  std::unique_ptr<FileManager> _file_manager;
  std::unique_ptr<LogManager> _log_manager;
  std::unique_ptr<BufferManager> _buffer_manager;
  std::unique_ptr<MetadataManager> _metadata_manager;
  std::unique_ptr<Planner> _planner;
};
} // namespace simpledb
