#include "jdbc/embeddedresultset.hpp"
#include "jdbc/embeddedconnection.hpp"
#include <algorithm>
#include <exception>
#include <stdexcept>
namespace simpledb {
EmbeddedResultSet::EmbeddedResultSet(Plan &plan, EmbeddedConnection &conn)
    : _conn(conn), _scan(plan.Open()), _schema(plan.GetSchema()) {}

bool EmbeddedResultSet::Next() {
  try {
    return _scan->Next();
  } catch (const std::exception &e) {
    _conn.Rollback();
    throw std::runtime_error(e.what());
  }
}

int EmbeddedResultSet::GetInt(const std::string &column_label) const {
  try {
    auto fieldname = column_label;
    std::transform(fieldname.begin(), fieldname.end(), fieldname.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return _scan->GetInt(fieldname);
  } catch (const std::exception &e) {
    _conn.Rollback();
    throw std::runtime_error(e.what());
  }
}

std::string
EmbeddedResultSet::GetString(const std::string &column_label) const {
  try {
    auto fieldname = column_label;
    std::transform(fieldname.begin(), fieldname.end(), fieldname.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return _scan->GetString(fieldname);
  } catch (const std::exception &e) {
    _conn.Rollback();
    throw std::runtime_error(e.what());
  }
}

EmbeddedMetaData *EmbeddedResultSet::GetMetaData() const {
  return new EmbeddedMetaData(_schema);
}

void EmbeddedResultSet::Close() {
  _scan->Close();
  _conn.Commit();
}
} // namespace simpledb
