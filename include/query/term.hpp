#pragma once

#include "expression.hpp"
#include "plan/plan.hpp"
namespace simpledb {

/// pg. 228 in the book explains the concept. pg. 232 has the implementation
///
/// A term is a comparison between two expressions.
class Term {
public:
  Term();
  Term(const Term &t);
  Term(const Expression &lhs, const Expression &rhs);
  Term &operator=(const Term &t);

  bool IsSatisfied(Scan *s) const;
  bool AppliesTo(const Schema &sch) const;
  /// Determine the expected number of records that will satisfy the predicate.
  int ReductionFactor(Plan *plan) const;
  Constant EquatesWithConstant(const std::string &fieldname) const;
  std::string EquatesWithField(const std::string &fieldname) const;
  std::string ToString() const;

private:
  Expression _lhs, _rhs;
};
} // namespace simpledb
