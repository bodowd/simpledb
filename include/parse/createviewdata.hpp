#pragma once
#include "parse/object.hpp"
#include "parse/querydata.hpp"
#include <string>

namespace simpledb {
class CreateViewData : public Object {
public:
  int op() override;
  CreateViewData(const std::string &viewname,
                 std::unique_ptr<QueryData> querydata);
  std::string ViewName() const;
  std::string ViewDef() const;

private:
  std::string _viewname;
  std::unique_ptr<QueryData> _querydata;
};

} // namespace simpledb
