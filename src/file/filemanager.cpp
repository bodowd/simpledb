#include "file/filemanager.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

namespace simpledb {
FileManager::FileManager(const std::filesystem::path &db_directory,
                         int block_size) {
  _db_directory = db_directory;
  _block_size = block_size;

  std::filesystem::directory_entry dir(_db_directory);
  _is_new = !dir.exists();
  if (_is_new) {
    std::filesystem::create_directory(_db_directory);
  }

  // remove leftover temp tables
  for (auto &i : std::filesystem::directory_iterator(_db_directory)) {
    if (i.path().filename().string().find("temp") == 0) {
      std::filesystem::remove(i.path());
    }
  }
}

// The Read method seeks to a position in the file and reads the contents of
// that block to the byte buffer of the page
void FileManager::Read(BlockId &blk, Page &p) {
  std::unique_lock<std::mutex> lock(_mutex);

  auto file = getFile(blk.Filename());
  auto offset = blk.Number() * _block_size;
  file->seekp(offset, std::ios::beg);

  file->read(&(*p.contents())[0], _block_size);
  if (file->bad()) {
    throw std::runtime_error("Error with I/O while reading");
  }
  int readCount = file->gcount();
  if (readCount < _block_size) {
    std::cerr << "Read less than a page" << std::endl;
    file->clear();
    memset(&(*p.contents())[0], 0, _block_size - readCount);
  }
}

// The Write method seeks to a position in the file and writes the contents
// of the page to the block at that position
void FileManager::Write(BlockId &blk, Page &p) {
  std::unique_lock<std::mutex> lock(_mutex);

  auto file = getFile(blk.Filename());
  auto offset = blk.Number() * _block_size;
  file->seekp(offset, std::ios::beg);
  file->write(&(*p.contents())[0], _block_size);
  if (file->bad()) {
    throw std::runtime_error("Error with I/O while writing");
  }
  file->flush();
}

// The Append method seeks to the end of the file and adds an empty array of
// bytes to it in order to get the OS to extend the file
BlockId FileManager::Append(std::string filename) {
  std::unique_lock<std::mutex> lock(_mutex);

  auto newBlockNum = Length(filename);
  BlockId blockid(filename, newBlockNum);

  auto file = getFile(filename);

  std::vector<char> emptyByteBuffer;
  emptyByteBuffer.resize(_block_size);
  auto offset = newBlockNum * _block_size;

  file->seekp(offset, std::ios::beg);
  file->write(&emptyByteBuffer[0], _block_size);
  if (file->bad()) {
    throw std::runtime_error("Error with I/O while Appending");
  }
  file->flush();
  return blockid;
}

std::shared_ptr<std::fstream>
FileManager::getFile(const std::string &filename) {
  auto file = std::make_shared<std::fstream>();
  std::filesystem::path path = _db_directory / filename;
  // std::fstream file(filename, std::ios::in | std::ios::out);

  // check if this file is already in the cache
  if (_open_files.find(path) != _open_files.end()) {
    file = _open_files[path];
    if (file->is_open()) {
      return file;
    }
  }

  // otherwise try to open the file. If it is not existing, create a new file
  file->open(path.string(), std::ios::binary | std::ios::in | std::ios::out);
  if (!file->is_open()) {
    // clear the error flag
    file->clear();
    // create a new file
    file->open(path.string(), std::ios::binary | std::ios::trunc |
                                  std::ios::in | std::ios::out);
    file->close();
    // re-open with original mode
    file->open(path.string(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file->is_open()) {
      throw std::runtime_error("can't open file");
    }
  }

  // store the opened file to the cache
  _open_files[path] = file;
  return file;
}

int FileManager::Length(const std::string filename) {
  std::filesystem::path path = _db_directory / filename;
  int size;
  try {
    size = std::filesystem::file_size(path);
  } catch (std::exception &e) {
    size = 0;
  }
  return size / _block_size;
}

bool FileManager::IsNew() { return _is_new; }
int FileManager::BlockSize() { return _block_size; }
} // namespace simpledb
