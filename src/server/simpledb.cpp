#include "server/simpledb.hpp"
#include "file/filemanager.hpp"
#include "index/planner/indexupdateplanner.hpp"
#include "log/logmanager.hpp"
#include "metadata/metadatamanager.hpp"
#include "plan/basicqueryplanner.hpp"
#include "plan/basicupdateplanner.hpp"
#include "plan/planner.hpp"
#include "tx/transaction.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace simpledb {

int SimpleDB::block_size = 400;
int SimpleDB::buffer_size = 8;
std::string SimpleDB::log_file = "simpledb.log";

SimpleDB::SimpleDB(const std::string dir_name, int block_size,
                   int buffer_size) {
  auto path = std::filesystem::current_path() / dir_name;
  _file_manager = std::make_unique<FileManager>(path, block_size);
  _log_manager =
      std::make_unique<LogManager>(_file_manager.get(), "simpledb.log");
  _buffer_manager = std::make_unique<BufferManager>(
      _file_manager.get(), _log_manager.get(), buffer_size);
};

SimpleDB::SimpleDB(const std::string &dir_name)
    : SimpleDB(dir_name, block_size, buffer_size) {
  auto tx = std::make_unique<Transaction>(
      _file_manager.get(), _log_manager.get(), _buffer_manager.get());
  bool isNew = _file_manager->IsNew();
  if (isNew) {
    std::cout << "creating new database" << std::endl;
  } else {
    std::cout << "recovering existing database" << std::endl;
    tx->Recover();
  }
  _metadata_manager = std::make_unique<MetadataManager>(isNew, tx.get());
  auto queryPlanner =
      std::make_unique<BasicQueryPlanner>(_metadata_manager.get());
  // auto updatePlanner =
  //     std::make_unique<BasicUpdatePlanner>(_metadata_manager.get());
  auto updatePlanner =
      std::make_unique<IndexUpdatePlanner>(_metadata_manager.get());
  _planner = std::make_unique<Planner>(std::move(queryPlanner),
                                       std::move(updatePlanner));
  tx->Commit();
}

Planner &SimpleDB::GetPlanner() { return *_planner; }

// std::unique_ptr<FileManager> SimpleDB::GetFileManager() {
//   return std::move(_file_manager);
// }

FileManager &SimpleDB::GetFileManager() { return *_file_manager; };

// std::unique_ptr<LogManager> SimpleDB::GetLogManager() {
//   return std::move(_log_manager);
// }

LogManager &SimpleDB::GetLogManager() { return *_log_manager; }

BufferManager &SimpleDB::GetBufferManager() { return *_buffer_manager; }

MetadataManager &SimpleDB::GetMetadataManager() { return *_metadata_manager; }

std::unique_ptr<Transaction> SimpleDB::NewTx() {
  return std::make_unique<Transaction>(_file_manager.get(), _log_manager.get(),
                                       _buffer_manager.get());
}

} // namespace simpledb
