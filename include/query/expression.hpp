#pragma once
#include "query/constant.hpp"
#include "query/scan.hpp"
#include "record/schema.hpp"
namespace simpledb {
/// pg. 231 in the book.
///
/// An expression consists of operations on constants and field names.
///
///
class Expression {
public:
  Expression();
  Expression(const Expression &e);
  Expression(const Constant &val);
  Expression(const std::string &fieldname);
  Expression &operator=(const Expression &e);
  bool IsFieldName() const;
  Constant AsConstant() const;
  std::string AsFieldName() const;
  // Returns the value of the expression with respect to a scan's current output
  // record. If the expression is a constant, the scan is irrelevant, and the
  // method returns the constant. If the expression is a field then the method
  // returns the field's value from the scan.
  Constant Evaluate(Scan *s) const;
  // Used by the query planner to determine the scope of the expression
  bool AppliesTo(const Schema &sch) const;
  std::string ToString() const;

private:
  Constant _val;
  std::string _fieldname;
};
} // namespace simpledb
