#pragma once
#include "parse/object.hpp"
#include "query/predicate.hpp"
#include <string>
namespace simpledb {

class DeleteData : public Object {
public:
  int op() override;
  DeleteData(const std::string &tablename, const Predicate &pred);
  std::string TableName() const;
  Predicate Pred() const;

private:
  std::string _tablename;
  Predicate _pred;
};
} // namespace simpledb
