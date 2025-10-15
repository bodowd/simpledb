#include "parse/createindexdata.hpp"
#include "parse/object.hpp"
namespace simpledb {
CreateIndexData::CreateIndexData(const std::string &indexname,
                                 const std::string &tablename,
                                 const std::string &fieldname)
    : _indexname(indexname), _tablename(tablename), _fieldname(fieldname) {}

int CreateIndexData::op() { return createindex; }

std::string CreateIndexData::IndexName() const { return _indexname; }

std::string CreateIndexData::TableName() const { return _tablename; }

std::string CreateIndexData::FieldName() const { return _fieldname; }
} // namespace simpledb
