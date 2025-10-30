#include "jdbc/embeddedconnection.hpp"
#include "jdbc/embeddedstatement.hpp"
#include "tx/transaction.hpp"
#include <memory>

namespace simpledb {
EmbeddedConnection::EmbeddedConnection(std::unique_ptr<SimpleDB> db)
    : _db(std::move(db)), _current_tx(_db->NewTx()),
      _planner(_db->GetPlanner()) {}

EmbeddedStatement *EmbeddedConnection::CreateStatement() {
  return new EmbeddedStatement(*this, _planner);
}

void EmbeddedConnection::Close() { _current_tx->Commit(); }

void EmbeddedConnection::Commit() {
  _current_tx->Commit();
  _current_tx = _db->NewTx();
}

void EmbeddedConnection::Rollback() {
  _current_tx->Rollback();
  _current_tx = _db->NewTx();
}

Transaction &EmbeddedConnection::GetTransaction() { return *_current_tx; }
} // namespace simpledb
