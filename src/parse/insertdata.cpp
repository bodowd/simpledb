#include "parse/insertdata.hpp"
#include <vector>
namespace simpledb {

InsertData::InsertData(const std::string &tablename,
                       const std::vector<std::string> &fields,
                       const std::vector<Constant> &vals)
    : _tablename(tablename), _fields(fields), _vals(vals) {}

int InsertData::op() { return insert; }

std::string InsertData::TableName() const { return _tablename; }

std::vector<std::string> InsertData::Fields() const { return _fields; }

std::vector<Constant> InsertData::Vals() const { return _vals; }

} // namespace simpledb
