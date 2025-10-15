#pragma once

#include "parse/object.hpp"
#include "record/schema.hpp"
#include <string>
namespace simpledb {
class CreateTableData : public Object {
public:
  int op() override;
  CreateTableData(const std::string &tablename, const Schema &sch);
  std::string TableName() const;
  Schema NewSchema() const;

private:
  std::string _tablename;
  Schema _schema;
};
} // namespace simpledb
