#pragma once

#include "jdbc/embeddedstatement.hpp"
#include "server/simpledb.hpp"
#include "tx/transaction.hpp"
#include <memory>

namespace simpledb {
class EmbeddedConnection {
public:
  EmbeddedConnection(std::unique_ptr<SimpleDB> db);
  EmbeddedStatement *CreateStatement();
  void Close();
  void Commit();
  void Rollback();
  Transaction &GetTransaction();

private:
  std::unique_ptr<SimpleDB> _db;
  std::unique_ptr<Transaction> _current_tx;
  Planner &_planner;
};
} // namespace simpledb
