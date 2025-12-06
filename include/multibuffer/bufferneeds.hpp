#pragma once

namespace simpledb {

/// p. 402 in the book
/// This class determines the best number of buffers to use for multibuffer
/// operators
class BufferNeeds {
public:
  /// Returns the best number of buffers to use that is less than the number of
  /// available buffers for the multibuffer sort operator
  ///
  /// available: number of available buffers size: the size of
  /// the table
  static int BestRoot(int available, int size);
  /// Returns the best number of buffers to use that is less than the number of
  /// available buffers for the multibuffer product operator
  ///
  /// available: number of available buffers size: the size of
  /// the table
  static int BestFactor(int available, int size);
};
} // namespace simpledb
