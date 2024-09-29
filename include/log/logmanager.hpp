#pragma once
#include "file/filemanager.hpp"
#include <memory>
#include <string>
#include <vector>

namespace simpledb {
class LogManager {
public:
  LogManager(FileManager *fm, std::string logfile);

  int Append(std::vector<char> &log_record);

  void Flush(int lsn);

  class LogIterator {
  public:
    LogIterator(FileManager *fm, BlockId blockId);
    bool HasNext();
    std::vector<char> Next();
    void MoveToBlock(BlockId blockId);

  private:
    FileManager *_file_manager;
    BlockId _block_id;
    std::unique_ptr<Page> _page;
    int _cur_pos;
    int _boundary;
  };

  LogIterator Iterator();

private:
  FileManager *_file_manager;
  std::string _log_file;
  std::unique_ptr<Page> _log_page;
  BlockId _cur_block;
  int _latest_lsn = 0;
  int _last_saved_lsn = 0;
  std::mutex _mutex;
  void flush();
  BlockId appendNewBlock();
};
} // namespace simpledb
