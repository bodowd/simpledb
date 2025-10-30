#include "jdbc/embeddedconnection.hpp"
namespace simpledb {
class EmbeddedDriver {
public:
  EmbeddedConnection *Connect(const std::string &filename);
};
} // namespace simpledb
