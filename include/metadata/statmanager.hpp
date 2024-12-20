#include "metadata/tablemangager.hpp"
#include "tx/transaction.hpp"
#include <map>
#include <mutex>
#include <string>
namespace simpledb {
/// p. 201 in the book
///
/// Only three kinds of statistical information are considered here:
/// The number of blocks used by each table, the number of records in each
/// table, and the number of distinct values for each field in the table.
class StatInfo {
public:
  StatInfo();
  StatInfo(const StatInfo &si);
  StatInfo(int numBlocks, int numRecs);
  StatInfo &operator=(const StatInfo &si);

  int BlocksAccessed();
  int RecordsOutput();
  int DistinctValues(const std::string &fieldname);

private:
  /// The number of blocks used by a table
  int _numBlocks;
  /// The number of records in a table
  int _numRecs;
};

/// p. 195 in the book.
///
/// Statistics are used by the query planner to estimate costs.
/// The StatManager manages this information.
///
/// The DB engine holds one StatManager object.
///
/// The statistics are stored directly in memory, and it requires the
/// statistical information to be recalculated by a function call every so
/// often. It does not update the statistics with every update to the database.
class StatManager {
public:
  StatManager(TableManager *tm, Transaction *tx);
  StatInfo GetStatInfo(const std::string &tablename, const Layout &layout,
                       Transaction *tx);

private:
  TableManager *_tm;
  /// Key: table name, Value: statistic information for that table
  std::map<std::string, StatInfo> _tablestats;
  /// Tracks the number of times StatInfo is called. When the counter reaches
  /// 100, refreshStastics will be called.
  int _numcalls;

  /// Allows the same thread to acquire the lock multiple times without causing
  /// a deadlock.
  std::recursive_mutex _mutex;

  /// Recalculate the cost values for all tables.
  void refreshStatistics(Transaction *tx);

  /// Calculate the statistics for the specified table.
  StatInfo calcTableStats(const std::string &tablename, const Layout &layout,
                          Transaction *tx);
};
} // namespace simpledb
