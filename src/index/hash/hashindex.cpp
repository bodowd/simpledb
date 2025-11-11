#include "index/hash/hashindex.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {
int HashIndex::NUM_BUCKETS = 100;

HashIndex::HashIndex(Transaction *tx, const std::string &indexName,
                     const Layout &layout)
    : _tx(tx), _indexname(indexName), _layout(layout) {}

void HashIndex::BeforeFirst(const Constant &searchKey) {
  Close();
  _search_key = searchKey;
  int bucket = _search_key.HashCode() % NUM_BUCKETS;
  std::string tablename = _indexname + std::to_string(bucket);
  _table_scan = std::make_unique<TableScan>(_tx, tablename, _layout);
}

bool HashIndex::Next() {
  while (_table_scan->Next()) {
    if (_table_scan->GetVal("dataval") == _search_key) {
      return true;
    }
  }
  return false;
}

RID HashIndex::GetDataRid() {
  int block_number = _table_scan->GetInt("block");
  int id = _table_scan->GetInt("id");
  return RID(block_number, id);
}

void HashIndex::Insert(const Constant &dataVal, const RID &dataRID) {
  BeforeFirst(dataVal);
  _table_scan->Insert();
  _table_scan->SetInt("block", dataRID.BlockNumber());
  _table_scan->SetInt("id", dataRID.Slot());
  _table_scan->SetVal("dataval", dataVal);
}

void HashIndex::Remove(const Constant &dataVal, const RID &dataRID) {
  BeforeFirst(dataVal);
  while (Next()) {
    if (GetDataRid() == dataRID) {
      _table_scan->Delete();
      return;
    }
  }
}

void HashIndex::Close() {
  if (_table_scan) {
    _table_scan->Close();
  }
}

int HashIndex::SearchCost(int numBlocks) { return numBlocks / NUM_BUCKETS; }
} // namespace simpledb
