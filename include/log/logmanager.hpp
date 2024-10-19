/*
 * We do not want to rely on Virtual Memory
 *
 * Not all virtual pages of memory fit in physical memory, so if there is not
 * enough of physical memory, the OS will spill to disk. This results in a
 * page swap when a process accesses a virtual page not in memory but on disk.
 * The OS will write a page of physical memory to disk in order to free a page
 * of physical memory and then write the contents of the page on disk into the
 * recently free physical memory. This is bad for the DB because we think
 * something is in memory but its not.
 *
 * We allocate a small number of pages that will fit in physical memory, and
 * I think this avoids page swaps? We do the swap ourselves because we know
 * better what pages our DB is using or not
 *
 *
 * */

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
