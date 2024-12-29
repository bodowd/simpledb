#include "query/term.hpp"
#include <algorithm>
#include <limits>

namespace simpledb {

Term::Term() {};

Term::Term(const Term &t) {
  _lhs = t._lhs;
  _rhs = t._rhs;
}

Term::Term(const Expression &lhs, const Expression &rhs)
    : _lhs(lhs), _rhs(rhs) {}

Term &Term::operator=(const Term &t) {
  if (this != &t) {
    _lhs = t._lhs;
    _rhs = t._rhs;
  }
  return *this;
}

bool Term::IsSatisfied(Scan *s) const {
  Constant lhsval = _lhs.Evaluate(s);
  Constant rhsval = _rhs.Evaluate(s);
  return lhsval == rhsval;
}

bool Term::AppliesTo(const Schema &sch) const {
  return _lhs.AppliesTo(sch) && _rhs.AppliesTo(sch);
}

int Term::ReductionFactor(Plan *plan) const {
  std::string lhsName, rhsName;

  if (_lhs.IsFieldName() && _rhs.IsFieldName()) {
    lhsName = _lhs.AsFieldName();
    rhsName = _lhs.AsFieldName();
    return std::max(plan->DistinctValues(lhsName),
                    plan->DistinctValues(rhsName));
  }

  if (_lhs.IsFieldName()) {
    lhsName = _lhs.AsFieldName();
    return plan->DistinctValues(lhsName);
  }

  if (_rhs.IsFieldName()) {
    rhsName = _rhs.AsFieldName();
    return plan->DistinctValues(rhsName);
  }

  // otherwise, the term is a constant
  if (_lhs.AsConstant() == _rhs.AsConstant()) {
    return 1;
  } else {
    return std::numeric_limits<int>::max();
  }
}

Constant Term::EquatesWithConstant(const std::string &fieldname) const {
  if (_lhs.IsFieldName() && _lhs.AsFieldName() == fieldname &&
      !_rhs.IsFieldName()) {
    return _rhs.AsConstant();
  } else if (_rhs.IsFieldName() && _rhs.AsFieldName() == fieldname &&
             !_lhs.IsFieldName()) {
    return _lhs.AsConstant();
  } else {
    Constant c;
    return c;
  }
}

std::string Term::EquatesWithField(const std::string &fieldname) const {
  if (_lhs.IsFieldName() && _lhs.AsFieldName() == fieldname &&
      _rhs.IsFieldName()) {
    return _rhs.AsFieldName();
  } else if (_rhs.IsFieldName() && _rhs.AsFieldName() == fieldname &&
             _lhs.IsFieldName()) {
    return _lhs.AsFieldName();
  } else {
    return "";
  }
}

std::string Term::ToString() const {
  return _lhs.ToString() + "=" + _rhs.ToString();
}

} // namespace simpledb
