#include "multibuffer/bufferneeds.hpp"
#include <cmath>
#include <limits>

namespace simpledb {

int BufferNeeds::BestRoot(int available, int size) {
  /// Reserve two buffers just in case
  int a = available - 2;
  if (a <= 1) {
    return 1;
  }
  int k = std::numeric_limits<int>::max();
  double i = 1.0;
  while (k > a) {
    i++;
    k = std::ceil(std::pow(static_cast<double>(size), 1 / i));
  }
  return k;
}

int BufferNeeds::BestFactor(int available, int size) {
  /// Reserve two buffers just in case
  int a = available - 2;
  if (a <= 1) {
    return 1;
  }
  int k = size;
  double i = 1.0;
  while (k > a) {
    i++;
    k = std::ceil(size / i);
  }
  return k;
}

} // namespace simpledb
