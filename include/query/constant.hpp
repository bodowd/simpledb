#pragma once
#include <memory>
#include <string>

namespace simpledb {

/// pg 228 in the book
///
/// A Constant is a value from a predetermined set of types such as integers and
/// strings. SimpleDB just has integer and string.
/// The class Constant will contain either an integer or string.
class Constant {
  friend bool operator==(const Constant &lhs, const Constant &rhs);
  friend bool operator!=(const Constant &lhs, const Constant &rhs);
  friend bool operator<(const Constant &lhs, const Constant &rhs);
  friend bool operator>(const Constant &lhs, const Constant &rhs);
  friend bool operator>=(const Constant &lhs, const Constant &rhs);
  friend bool operator>=(const Constant &lhs, const Constant &rhs);

public:
  Constant();
  Constant(const Constant &val);
  Constant(int val);
  Constant(const std::string &val);
  Constant &operator=(const Constant &val);
  int AsInt() const;
  std::string AsString() const;
  std::string ToString() const;
  int HashCode() const;
  bool IsNull() const;

private:
  std::unique_ptr<int> _ival;
  std::unique_ptr<std::string> _sval;
};
} // namespace simpledb
