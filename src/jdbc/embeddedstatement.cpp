#include "jdbc/embeddedstatement.hpp"
#include "jdbc/embeddedconnection.hpp"
#include "jdbc/embeddedresultset.hpp"
#include "tx/transaction.hpp"
#include <exception>
#include <stdexcept>
namespace simpledb {
EmbeddedStatement::EmbeddedStatement(EmbeddedConnection &conn, Planner &planner)
    : _conn(conn), _planner(planner) {};

EmbeddedResultSet *EmbeddedStatement::ExecuteQuery(const std::string &sql) {
  try {
    Transaction &tx = _conn.GetTransaction();
    auto query_plan = _planner.CreateQueryPlan(sql, &tx);
    return new EmbeddedResultSet(*query_plan, _conn);
  } catch (const std::exception &e) {
    _conn.Rollback();
    throw std::runtime_error(e.what());
  }
}

int EmbeddedStatement::ExecuteUpdate(const std::string &sql) {
  try {
    Transaction &tx = _conn.GetTransaction();
    int result = _planner.ExecuteUpdate(sql, &tx);
    _conn.Commit();
    return result;
  } catch (const std::exception &e) {
    _conn.Rollback();
    throw std::runtime_error(e.what());
  }
}

void EmbeddedStatement::Close() { return; }
} // namespace simpledb
