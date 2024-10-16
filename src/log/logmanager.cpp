#include "log/logmanager.hpp"
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

namespace simpledb {
LogManager::LogManager(FileManager *fm, std::string logfile) {
  _file_manager = fm;
  _log_file = logfile;
  auto byte_array =
      std::make_shared<std::vector<char>>(_file_manager->BlockSize(), 0);
  _log_page = std::make_unique<Page>(byte_array);
  auto log_size = _file_manager->Length(_log_file);
  if (log_size == 0) {
    _cur_block = appendNewBlock();
  } else {
    _cur_block = BlockId(_log_file, log_size - 1);
    // bring the existing log into memory by reading it into a page
    _file_manager->Read(_cur_block, *_log_page);
  }
}

LogManager::LogIterator::LogIterator(FileManager *fm, BlockId blockId)
    : _file_manager(fm), _block_id(blockId) {
  auto byte_array =
      std::make_shared<std::vector<char>>(_file_manager->BlockSize(), 0);
  _page = std::make_unique<Page>(byte_array);
  MoveToBlock(_block_id);
}

void LogManager::LogIterator::MoveToBlock(BlockId blockId) {
  _file_manager->Read(blockId, *_page);
  _boundary = _page->GetInt(0);
  _cur_pos = _boundary;
}

// The iterator moves backwards through the log records starting with the
// higher block number towards block number zero.
// It returns false if there are no more records in the page or if there are no
// more previous blocks
bool LogManager::LogIterator::HasNext() {
  // if _cur_pos is smaller than the block size, there are still records to go
  // through in the page
  return _cur_pos < _file_manager->BlockSize() || _block_id.Number() > 0;
}

std::vector<char> LogManager::LogIterator::Next() {
  if (_cur_pos == _file_manager->BlockSize()) {
    _block_id = BlockId(_block_id.Filename(), _block_id.Number() - 1);
    MoveToBlock(_block_id);
  }

  std::vector<char> record = _page->GetBytes(_cur_pos);
  _cur_pos += sizeof(int) + record.size();
  return record;
}

LogManager::LogIterator LogManager::Iterator() {
  flush();
  return LogManager::LogIterator(_file_manager, _cur_block);
}

// Log records are written from right to left.
// The first 4 bytes of the page contain the boundary value.
// So a log page looks like this:
//
// 0   4                 20
// ┌───┌─────────────────┬───────────┐
// │20 │                 │log records│
// └───└─────────────────┴───────────┘
//  ▲
//  │
//  │
//  │
//  boundary value is stored here
//  In this case, the boundary is
//  at position 20
//
//
// boundary value 4 bytes - empty space - newest record - older records
// - end of page
int LogManager::Append(std::vector<char> &log_record) {
  std::unique_lock<std::mutex> lock(_mutex);

  // The boundary value contains the offset of the most recently added record.
  // The boundary value is written to the first 4 bytes of the page
  // so that the iterator will now where the records begin
  auto boundary = _log_page->GetInt(0);
  auto record_size = log_record.size();
  // A new insert in the page requires the record itself as well as the size of
  // that record, which is the header for that entry in the page
  int bytes_needed = record_size + sizeof(int);

  // Recall, the log page writes right to left.
  // If the boundary is at offset 10, and the bytes needed to write the
  // next log record is 8, it will overflow into the boundary value storage,
  // which is to the left, in the first 4 bytes of the page because it would be
  // 4 bytes for boundary value, then 6 bytes to get to offset 10 which means
  // there are only 6 bytes available in the page, but 8 bytes are needed. So it
  // won't fit.
  //
  // 0   4                 10
  // ┌───┌─────────────────┬───────────┐
  // │ xxxxxxxxxxxxxxxxxxxx│log records│
  // └───└─────────────────┴───────────┘
  //                   ▲
  //                   │
  //                   │
  //                   │
  //                   new log record of  8 bytes
  //                   would overflow into the boundary
  //                   region because there are only 6
  //                   bytes of free space in between
  //                   the boundary value storage region
  //                   and the last log record
  //
  // boundary value (4B) -- empty space (6B) -- offset at position 10.
  // 8 bytes needed, but only 6B of empty space
  if (boundary - bytes_needed < static_cast<int>(sizeof(int))) {
    // the new record won't fit in this page. Flush the current page to disk
    // to persist the contents before making a new page in the log file, below
    flush();
    _cur_block = appendNewBlock();
    boundary = _log_page->GetInt(0);
  }

  // log page is written right to left
  int record_position = boundary - bytes_needed;
  _log_page->SetBytes(record_position, log_record);
  // update the boundary value
  _log_page->SetInt(0, record_position);
  _latest_lsn += 1;
  return _latest_lsn;
};

// Extends the log file by appending a new block to the page.
// Clears the log page before and then writes the now empty page
// to the block (thereby adding it to the file)
BlockId LogManager::appendNewBlock() {
  auto newBlock = _file_manager->Append(_log_file);
  _log_page->SetInt(0, _file_manager->BlockSize());
  _file_manager->Write(newBlock, *_log_page);
  return newBlock;
}

void LogManager::Flush(int lsn) {
  std::unique_lock<std::mutex> lock(_mutex);
  if (lsn >= _last_saved_lsn) {
    flush();
  }
}

void LogManager::flush() {
  _file_manager->Write(_cur_block, *_log_page);
  _last_saved_lsn = _latest_lsn;
}
} // namespace simpledb
