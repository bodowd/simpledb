#pragma once

#include "parse/object.hpp"
#include "query/constant.hpp"
#include <string>
#include <vector>

namespace simpledb {

class InsertData : public Object {
public:
  int op() override;
  InsertData(const std::string &tablename,
             const std::vector<std::string> &fields,
             const std::vector<Constant> &vals);
  std::string TableName() const;
  std::vector<std::string> Fields() const;
  std::vector<Constant> Vals() const;

private:
  std::string _tablename;
  std::vector<std::string> _fields;
  std::vector<Constant> _vals;
};

} // namespace simpledb
