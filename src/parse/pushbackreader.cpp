#include "parse/pushbackreader.hpp"
#include <istream>
#include <stdexcept>
namespace simpledb {

PushBackReader::PushBackReader(std::istream &is) : _currentPos(0) {
  /// get the length of the input
  /// seekg - move the "get pointer" to a position
  is.seekg(0, is.end);
  /// tellg - tell the "get pointer" position
  int length = is.tellg();
  is.seekg(0, is.beg);

  _buffer.resize(length);
  is.read(&_buffer[0], length);
}

int PushBackReader::Read() {
  if (_currentPos == static_cast<int>(_buffer.size())) {
    return -1;
  }
  return static_cast<int>(_buffer[_currentPos++]);
}

void PushBackReader::Unread(int ch) {
  if (_currentPos == 0) {
    throw std::runtime_error("push back buffer is full");
  }
  /// decrement first, and then use _currentPos
  _buffer[--_currentPos] = static_cast<char>(ch);
}
} // namespace simpledb
