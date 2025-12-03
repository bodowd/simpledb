#pragma once

#include "materialize/recordcomparator.hpp"
#include "materialize/temptable.hpp"
#include "plan/plan.hpp"

#include "query/updatescan.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {

/// p. 377 in the book
/// Implements mergesort
class SortPlan : public Plan {
public:
  SortPlan(Transaction *tx, const std::shared_ptr<Plan> &plan,
           const std::vector<std::string> &sortFields);
  /// The Open() method performs the mergesort. It merges two runs at a time and
  /// does not attempt to reduce the number of initial runs
  std::shared_ptr<Scan> Open() override;
  int BlocksAccessed() override;
  int RecordsOutput() override;
  int DistinctValues(const std::string &fieldName) override;
  Schema GetSchema() override;

private:
  std::shared_ptr<Plan> _plan;
  Transaction *_tx;
  Schema _sch;
  RecordComparator _comp;

  /// A run is a sorted portion of a table. For example:
  /// 2, 6, 20, 4, 1, 16, 19, 3, 18
  /// 2, 6, 20 is the first run
  /// 4 is the second
  /// 1, 16, 19  is the third and
  /// 3, 18 is the fourth
  /// Creates several temporary tables by splitting the input scan into runs.
  /// Does not do any sorting; it just splits the table into its already sorted
  /// portions
  std::vector<std::shared_ptr<TempTable>> splitIntoRuns(Scan *src);

  /// Merges each pair of temporary tables it is given and returns
  /// a vector containing the resulting (merged) temporary tables
  std::vector<std::shared_ptr<TempTable>>
  doAMergeIteration(const std::vector<std::shared_ptr<TempTable>> &runs);

  std::shared_ptr<TempTable> mergeTwoRuns(TempTable *p1, TempTable *p2);

  /// Copy a tuple from src to dest.
  /// If there are more records in the src Scan, returns True. If there are no
  /// more records in the src Scan, returns False
  bool copy(Scan *src, UpdateScan *dest);
};
} // namespace simpledb
