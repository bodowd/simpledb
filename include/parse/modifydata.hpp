#pragma once
#include "parse/object.hpp"
#include "query/expression.hpp"
#include "query/predicate.hpp"

namespace simpledb {
class ModifyData : public Object {
public:
  int op() override;
  ModifyData(const std::string &tablename, const std::string &fieldname,
             const Expression &newVal, const Predicate &pred);
  std::string TableName() const;
  std::string TargetField() const;
  Expression NewVal() const;
  Predicate Pred() const;

private:
  std::string _tablename;
  std::string _fieldname;
  Expression _newVal;
  Predicate _pred;
};
} // namespace simpledb
