#pragma once

#include "metadata/indexmanager.hpp"
#include "plan/plan.hpp"
#include <memory>

namespace simpledb {

/// p. 350 in the book
///
/// An index join is a special case of a regular join implementation where table
/// 2 is a stored table having an index on the join field. It is similar to a
/// product between the two tables. However, instead of repeatedly scanning the
/// inner table, the code only has to repeatedly search the index
class IndexJoinPlan : public Plan {
public:
  /// plan1 is the plan for table 1 in the join, plan 2 is the plan for table 2
  /// in the join.
  /// indexInfo is table 2's index on the joinField in table 2
  /// joinField corresponds to the joinField in table 1
  ///
  IndexJoinPlan(const std::shared_ptr<Plan> &plan1,
                const std::shared_ptr<Plan> &plan2, const IndexInfo &indexInfo,
                const std::string &joinField);

  /// Convert the plans to scans and the IndexInfo object to an index. Then will
  /// pass these to an IndexJoinScan constructor in order to make the scan
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan_1, _plan_2;
  IndexInfo _index_info;
  std::string _join_field;
  Schema _sch;
};
} // namespace simpledb
