#include "file/page.hpp"
#include <cstring>
#include <memory>
#include <stdexcept>

namespace simpledb {
/*
 * At a certain offset, there is a byte sequence stored in the page.
 * Each byte sequence has a header that stores the size of the byte sequence
 * followed by the byte sequence itself.
 * len, byte sequence
 *
 * */

Page::Page(int block_size) {
  _byte_buffer = std::make_shared<std::vector<char>>(block_size);
};

Page::Page(const std::shared_ptr<std::vector<char>> &byte_buffer)
    : _byte_buffer(byte_buffer) {};

int Page::GetInt(int offset) {

  // since the _byte_buffer is a std::shared_ptr pointing to the vector
  // container _byte_buffer.get() accesses the std::vector container, but not
  // its contents.
  // Another way is to use ->size() which directly dereferences the shared_ptr
  // and gets the size of the vector that it points to.
  // In this check, we want to get the container, not the contents. We want to
  // find the size of the std::vector container
  if (offset + sizeof(int) > _byte_buffer->size()) {
    throw std::runtime_error("Page overflow in GetInt");
  }

  int num;
  // _byte_buffer->data() accesses the contents of the std::vector container,
  // and not the container itself. We want to access the contents, and then go
  // to a particular offset
  std::memcpy(&num, _byte_buffer->data() + offset, sizeof(int));

  return num;
}

void Page::SetBytes(int offset, std::vector<char> &byte_buffer) {
  // SetBytes will allow a "byte array" to be stored.
  // The format stored in the Page will be the size of the std::vector<char>
  // to be stored, followed by std::vector<char> (which is the byte array).
  // The size of the byte array needs to be known so we know how far to read
  // in the page when retrieving the bytes later.

  auto size = byte_buffer.size();
  if (offset + sizeof(int) + size > _byte_buffer->size()) {
    throw std::runtime_error("Page overflow in SetBytes");
  }
  // store the size of the bytes array being stored followed by the bytes
  // themselves
  SetInt(offset, size);
  memcpy(_byte_buffer->data() + offset + sizeof(int), &byte_buffer, size);
}

std::vector<char> Page::GetBytes(int offset) {
  // GetBytes will retrieve a sequence of bytes that was stored in the page
  // The page stores this first with the length (size) of the sequence of bytes,
  // followed by the bytes themselves.

  // First, get the size of the byte sequence so that we know
  // how big the byte sequence at this offset is, and then we know
  // how much to read to get this byte sequence
  auto size = GetInt(offset);

  if (offset + sizeof(int) + size > _byte_buffer->size()) {
    throw std::runtime_error("Page overflow in GetBytes");
  }

  std::vector<char> byte_vec;
  // Copy the bytes that are stored in the page.
  // The byte sequence starts one byte after the offset because the offset
  // stores the size of the sequence.
  byte_vec.insert(byte_vec.end(), _byte_buffer->data() + offset + sizeof(int),
                  _byte_buffer->data() + offset + sizeof(int) + size);
  return byte_vec;
}

std::string Page::GetString(int offset) {
  auto b = GetBytes(offset);
  return std::string(b.begin(), b.end());
}

void Page::SetInt(int offset, int val) {
  if (offset + sizeof(int) > _byte_buffer->size()) {
    throw std::runtime_error("Page overflow in SetInt");
  }
  memcpy(_byte_buffer->data() + offset, &val, sizeof(int));
}

void Page::SetString(int offset, std::string val) {
  auto v = std::vector<char>(val.begin(), val.end());
  return SetBytes(offset, v);
}

int Page::MaxLength(int strlen) { return sizeof(int) + strlen; };

std::shared_ptr<std::vector<char>> Page::contents() { return _byte_buffer; }

} // namespace simpledb
