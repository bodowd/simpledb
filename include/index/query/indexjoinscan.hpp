#pragma once

#include "index/index.hpp"
#include "query/scan.hpp"
#include "record/tablescan.hpp"
#include <memory>
namespace simpledb {
// In the simpledb index join scan, the right hand side (aka t2) is the side of
// the join with an index on the join field
class IndexJoinScan : public Scan {
public:
  /// t1 is the left hand side table and t2 is the right hand side table
  /// index refers to the index on the joinField in t2
  /// joinField refers to the field in t1 that the join should occur on
  IndexJoinScan(const std::shared_ptr<Scan> &left_hand_side,
                const std::shared_ptr<Index> &index,
                const std::string &joinField,
                const std::shared_ptr<TableScan> &right_hand_side);
  /// Position the index before the data value of the
  void BeforeFirst() override;
  /// Move to the next index value (of t2 -- the right hand side table) if one
  /// exists. Otherwise, move to the next value of t1 and also reset the index
  /// to point to the next index value (the index is in t2)
  bool Next() override;
  int GetInt(const std::string &fieldName) override;
  std::string GetString(const std::string &fieldName) override;
  Constant GetVal(const std::string &fieldName) override;
  bool HasField(const std::string &fieldName) override;
  void Close() override;
  /// Reset the index to point to the position right before the new data value
  void ResetIndex();

private:
  std::shared_ptr<Scan> _left_hand_side;
  std::shared_ptr<Index> _index;
  std::string _join_field;
  std::shared_ptr<TableScan> _right_hand_side;
};
} // namespace simpledb
