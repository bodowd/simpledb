#pragma once

#include "parse/object.hpp"
#include <string>

namespace simpledb {
class CreateIndexData : public Object {
public:
  int op() override;
  CreateIndexData(const std::string &indexname, const std::string &tablename,
                  const std::string &fieldname);
  std::string IndexName() const;
  std::string TableName() const;
  std::string FieldName() const;

private:
  std::string _indexname;
  std::string _tablename;
  std::string _fieldname;
};
} // namespace simpledb
