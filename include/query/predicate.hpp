#pragma once
#include "query/constant.hpp"
#include "term.hpp"
#include <vector>
namespace simpledb {

/// pg. 234 in the book and pg 228 contains explanations about the Predicate
/// concept.
///
/// A predicate is a condition that returns true or false for each row of a
/// given scan. It would correspond to the WHERE statement of a SQL query.
///
/// A predicate is either a term or the boolean combination of terms. A term is
/// a comparison between two expressions.
class Predicate {
public:
  Predicate();
  Predicate(const Predicate &pred);
  Predicate(const Term &term);
  Predicate &operator=(const Predicate &pred);
  bool IsNull() const;
  void ConjoinWith(const Predicate &pred);
  bool IsSatisfied(Scan *s) const;
  int ReductionFactor(Plan *plan) const;
  Predicate SelectSubPred(const Schema &sch) const;
  Predicate JoinSubPred(const Schema &sch1, const Schema &sch2) const;
  Constant EquatesWithConstant(const std::string &fieldname) const;
  std::string EquatesWithField(const std::string &fieldname) const;
  std::string ToString() const;

private:
  // A predicate is implemented as a list of terms, and a predicate responds to
  // its methods by calling the corresponding methods on each of its terms
  std::vector<Term> _terms;
};
} // namespace simpledb
