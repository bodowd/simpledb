#include "query/constant.hpp"
#include <memory>

namespace simpledb {
bool operator==(const Constant &lhs, const Constant &rhs) {
  return lhs._ival ? *lhs._ival == *rhs._ival : *lhs._sval == *rhs._sval;
}

bool operator!=(const Constant &lhs, const Constant &rhs) {
  if (lhs == rhs) {
    return false;
  }
  return true;
}

bool operator<(const Constant &lhs, const Constant &rhs) {
  return lhs._ival ? *lhs._ival < *rhs._ival : *lhs._sval < *rhs._sval;
}

bool operator>(const Constant &lhs, const Constant &rhs) {
  return lhs._ival ? *lhs._ival > *rhs._ival : *lhs._sval > *rhs._sval;
}

bool operator<=(const Constant &lhs, const Constant &rhs) {
  if (lhs > rhs) {
    return false;
  }
  return true;
}

bool operator>=(const Constant &lhs, const Constant &rhs) {
  if (lhs < rhs) {
    return false;
  }
  return true;
}

Constant::Constant() {}

Constant::Constant(const Constant &val) {
  if (val._ival) {
    _ival = std::make_unique<int>(*(val._ival));
  }
  if (val._sval) {
    _sval = std::make_unique<std::string>(*(val._sval));
  }
}

Constant::Constant(int val) { _ival = std::make_unique<int>(val); }

Constant::Constant(const std::string &val) {
  _sval = std::make_unique<std::string>(val);
}

Constant &Constant::operator=(const Constant &val) {
  if (this != &val) {
    if (val._ival) {
      _ival = std::make_unique<int>(*(val._ival));
    }
    if (val._sval) {
      _sval = std::make_unique<std::string>(*(val._sval));
    }
  }
  return *this;
}

int Constant::AsInt() const { return *_ival; }

std::string Constant::AsString() const { return *_sval; }

std::string Constant::ToString() const {
  return _ival ? std::to_string(*_ival) : *_sval;
}

int Constant::HashCode() const {
  return _ival ? std::hash<int>{}(*_ival) : std::hash<std::string>{}(*_sval);
}

bool Constant::IsNull() const {
  if (_ival || _sval) {
    return false;
  }
  return true;
}

} // namespace simpledb
