#include "jdbc/embeddeddriver.hpp"
#include "jdbc/embeddedconnection.hpp"

namespace simpledb {
EmbeddedConnection *EmbeddedDriver::Connect(const std::string &filename) {
  auto db = std::make_unique<SimpleDB>(filename);
  return new EmbeddedConnection(std::move(db));
};
} // namespace simpledb
