#pragma once

#include "query/constant.hpp"
#include "query/scan.hpp"
namespace simpledb {
class AggregationFn {
public:
  ~AggregationFn() {};
  virtual void ProcessFirst(Scan *s) = 0;
  virtual void ProcessNext(Scan *s) = 0;
  virtual std::string FieldName() = 0;
  virtual Constant Value() = 0;
};
} // namespace simpledb
