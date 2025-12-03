#pragma once

#include "materialize/aggregationfn.hpp"
#include "materialize/groupvalue.hpp"
#include "query/scan.hpp"
#include <memory>
#include <vector>

namespace simpledb {
class GroupByScan : public Scan {
public:
  GroupByScan(
      const std::shared_ptr<Scan> &scan,
      const std::vector<std::string> &groupFields,
      const std::vector<std::shared_ptr<AggregationFn>> &aggregationFunctions);
  void BeforeFirst() override;
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  void Close() override;

private:
  std::shared_ptr<Scan> _scan;
  std::vector<std::string> _group_fields;
  std::vector<std::shared_ptr<AggregationFn>> _aggregation_functions;
  std::unique_ptr<GroupValue> _group_val;
  bool _more_groups;
};
} // namespace simpledb
