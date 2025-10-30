#pragma once

#include "jdbc/embeddedmetadata.hpp"
#include "plan/plan.hpp"
#include "query/scan.hpp"

namespace simpledb {
class EmbeddedConnection;
class EmbeddedResultSet {
public:
  EmbeddedResultSet(Plan &plan, EmbeddedConnection &conn);
  bool Next();
  int GetInt(const std::string &column_label) const;
  std::string GetString(const std::string &column_label) const;
  EmbeddedMetaData *GetMetaData() const;
  void Close();

private:
  EmbeddedConnection &_conn;
  std::shared_ptr<Scan> _scan;
  Schema _schema;
};
} // namespace simpledb
