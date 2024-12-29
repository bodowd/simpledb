#pragma once
#include "query/scan.hpp"
#include <memory>
#include <vector>
namespace simpledb {

/// p. 224 in the book
///
/// Projection: get the columns specified
class ProjectScan : public Scan {
public:
  ProjectScan(const std::shared_ptr<Scan> &scan,
              const std::vector<std::string> &fieldlist);

  void BeforeFirst() override;
  bool Next() override;
  int GetInt(const std::string &fieldname) override;
  std::string GetString(const std::string &fieldname) override;
  Constant GetVal(const std::string &fieldname) override;
  bool HasField(const std::string &fieldname) override;
  void Close() override;

private:
  std::shared_ptr<Scan> _scan;
  /// list of output fields for this Project scan to look for and output if
  /// present
  std::vector<std::string> _fieldlist;
};

} // namespace simpledb
