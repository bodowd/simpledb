#include "parse/createtabledata.hpp"
#include "parse/object.hpp"
namespace simpledb {
CreateTableData::CreateTableData(const std::string &tablename,
                                 const Schema &sch)
    : _tablename(tablename), _schema(sch) {}

int CreateTableData::op() { return createtable; }

std::string CreateTableData::TableName() const { return _tablename; }

Schema CreateTableData::NewSchema() const { return _schema; }
} // namespace simpledb
