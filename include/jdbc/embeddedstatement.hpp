#pragma once

#include "jdbc/embeddedresultset.hpp"
#include "plan/planner.hpp"
namespace simpledb {
class EmbeddedConnection;
class EmbeddedStatement {
public:
  EmbeddedStatement(EmbeddedConnection &conn, Planner &planner);
  EmbeddedResultSet *ExecuteQuery(const std::string &sql);
  int ExecuteUpdate(const std::string &sql);
  void Close();

private:
  EmbeddedConnection &_conn;
  Planner &_planner;
};
} // namespace simpledb
