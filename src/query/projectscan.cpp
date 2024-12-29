#include "query/projectscan.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
namespace simpledb {

ProjectScan::ProjectScan(const std::shared_ptr<Scan> &scan,
                         const std::vector<std::string> &fieldlist)
    : _scan(scan), _fieldlist(fieldlist) {}

void ProjectScan::BeforeFirst() { _scan->BeforeFirst(); }

bool ProjectScan::Next() { return _scan->Next(); }

int ProjectScan::GetInt(const std::string &fieldname) {
  if (HasField(fieldname)) {
    return _scan->GetInt(fieldname);
  } else {
    throw std::runtime_error("field not found");
  }
}

std::string ProjectScan::GetString(const std::string &fieldname) {
  if (HasField(fieldname)) {
    return _scan->GetString(fieldname);
  } else {
    throw std::runtime_error("field not found");
  }
}

Constant ProjectScan::GetVal(const std::string &fieldname) {
  if (HasField(fieldname)) {
    return _scan->GetVal(fieldname);
  } else {
    throw std::runtime_error("field not found");
  }
}

bool ProjectScan::HasField(const std::string &fieldname) {
  return std::find(_fieldlist.begin(), _fieldlist.end(), fieldname) !=
         _fieldlist.end();
}

void ProjectScan::Close() { _scan->Close(); }
} // namespace simpledb
