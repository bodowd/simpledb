#include "query/scan.hpp"
#include "record/schema.hpp"
#include <memory>

namespace simpledb {
class Plan {
public:
  virtual ~Plan() {}
  virtual std::shared_ptr<Scan> Open() = 0;
  virtual int BlocksAccessed() = 0;
  virtual int RecordsOutput() = 0;
  virtual int DistinctValues(const std::string &fieldname) = 0;
  virtual Schema GetSchema() = 0;
};
} // namespace simpledb
