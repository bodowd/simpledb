#pragma once

#include "materialize/aggregationfn.hpp"
namespace simpledb {

class MaxFn : public AggregationFn {
public:
  MaxFn(const std::string &fieldName);
  void ProcessFirst(Scan *scan) override;
  void ProcessNext(Scan *scan) override;
  std::string FieldName() override;
  Constant Value() override;

private:
  std::string _field_name;
  Constant _val;
};
} // namespace simpledb
