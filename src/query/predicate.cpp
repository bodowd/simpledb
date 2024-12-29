#include "query/predicate.hpp"

namespace simpledb {

Predicate::Predicate() {}

Predicate::Predicate(const Predicate &pred) { _terms = pred._terms; }

Predicate::Predicate(const Term &term) { _terms.emplace_back(term); }

Predicate &Predicate::operator=(const Predicate &pred) {
  if (this != &pred) {
    _terms = pred._terms;
  }
  return *this;
}

bool Predicate::IsNull() const { return _terms.empty(); }

void Predicate::ConjoinWith(const Predicate &pred) {
  _terms.insert(_terms.end(), pred._terms.begin(), pred._terms.end());
}

bool Predicate::IsSatisfied(Scan *s) const {
  for (const Term &t : _terms) {
    if (!t.IsSatisfied(s)) {
      return false;
    }
  }
  return true;
}

int Predicate::ReductionFactor(Plan *plan) const {
  int factor = 1;
  for (const Term &t : _terms) {
    factor *= t.ReductionFactor(plan);
  }
  return factor;
}

Predicate Predicate::SelectSubPred(const Schema &sch) const {
  Predicate result;
  for (const Term &t : _terms) {
    if (t.AppliesTo(sch)) {
      result._terms.emplace_back(t);
    }
  }
  return result;
}

Predicate Predicate::JoinSubPred(const Schema &sch1, const Schema &sch2) const {
  Predicate result;
  Schema newSch;
  newSch.AddAll(sch1);
  newSch.AddAll(sch2);

  // add any terms to the new schema that are not already captured by sch1 and
  // sch2, but are in the current Predicate
  for (const Term &t : _terms) {
    if (!t.AppliesTo(sch1) && !t.AppliesTo(sch2) && t.AppliesTo(newSch)) {
      result._terms.emplace_back(t);
    }
  }

  return result;
}

Constant Predicate::EquatesWithConstant(const std::string &fieldname) const {
  for (const Term &t : _terms) {
    Constant c = t.EquatesWithConstant(fieldname);
    if (!c.IsNull()) {
      return c;
    }
  }
  Constant nullConstant;
  return nullConstant;
}

std::string Predicate::EquatesWithField(const std::string &fieldname) const {
  for (const Term &t : _terms) {
    std::string s = t.EquatesWithField(fieldname);
    if (!s.empty()) {
      return s;
    }
  }
  return "";
}

std::string Predicate::ToString() const {
  std::string result = "";
  int count = 0;
  for (const Term &t : _terms) {
    count++;
    result += t.ToString();
    if (count < static_cast<int>(_terms.size())) {
      result += " and ";
    }
  }
  return result;
}
} // namespace simpledb
