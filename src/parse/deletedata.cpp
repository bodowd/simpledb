#include "parse/deletedata.hpp"

namespace simpledb {
DeleteData::DeleteData(const std::string &tablename, const Predicate &pred)
    : _tablename(tablename), _pred(pred) {}

int DeleteData::op() { return remove; }

std::string DeleteData::TableName() const { return _tablename; }

Predicate DeleteData::Pred() const { return _pred; }
} // namespace simpledb
