#include "metadata/statmanager.hpp"
#include "metadata/tablemangager.hpp"
#include "record/tablescan.hpp"
#include "tx/transaction.hpp"
#include <mutex>
namespace simpledb {

StatInfo::StatInfo() {}

StatInfo::StatInfo(const StatInfo &si) {
  _numBlocks = si._numBlocks;
  _numRecs = si._numRecs;
}

StatInfo::StatInfo(int numBlocks, int numRecs)
    : _numBlocks(numBlocks), _numRecs(numRecs) {}

StatInfo &StatInfo::operator=(const StatInfo &si) {
  if (this != &si) {
    _numBlocks = si._numBlocks;
    _numRecs = si._numRecs;
  }
  return *this;
}

int StatInfo::BlocksAccessed() { return _numBlocks; }

int StatInfo::RecordsOutput() { return _numRecs; }

int StatInfo::DistinctValues(const std::string &fieldname) {
  return 1 + (_numRecs / 3); // can be improved
}

StatManager::StatManager(TableManager *tm, Transaction *tx) {
  _tm = tm;
  refreshStatistics(tx);
}

StatInfo StatManager::GetStatInfo(const std::string &tablename,
                                  const Layout &layout, Transaction *tx) {
  std::unique_lock<std::recursive_mutex> lock(_mutex);
  _numcalls++;
  if (_numcalls > 100) {
    refreshStatistics(tx);
  }

  StatInfo si;
  if (_tablestats.find(tablename) == _tablestats.end()) {
    si = calcTableStats(tablename, layout, tx);
    _tablestats[tablename] = si;
  } else {
    si = _tablestats[tablename];
  }
  return si;
}

void StatManager::refreshStatistics(Transaction *tx) {
  std::unique_lock<std::recursive_mutex> lock(_mutex);
  _tablestats.clear();
  _numcalls = 0;

  Layout tcatLayout = _tm->GetLayout(_tm->TABLECAT, tx);
  TableScan tcat(tx, _tm->TABLECAT, tcatLayout);
  while (tcat.Next()) {
    std::string tablename = tcat.GetString(_tm->TABLENAME);
    Layout layout = _tm->GetLayout(tablename, tx);
    StatInfo si = calcTableStats(tablename, layout, tx);
    _tablestats[tablename] = si;
  }
  tcat.Close();
};

StatInfo StatManager::calcTableStats(const std::string &tablename,
                                     const Layout &layout, Transaction *tx) {
  int numRecs = 0;
  int numBlocks = 0;
  TableScan ts(tx, tablename, layout);
  while (ts.Next()) {
    numRecs++;
    numBlocks = ts.GetRid().BlockNumber() + 1;
  }
  ts.Close();
  return StatInfo(numBlocks, numRecs);
}
} // namespace simpledb
