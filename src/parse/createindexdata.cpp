#include "parse/createindexdata.hpp"
#include "parse/object.hpp"
namespace simpledb {
CreateIndexData::CreateIndexData(const std::string &indexname,
                                 const std::string &tablename,
                                 const std::string &fieldname,
                                 const std::string &indextype)
    : _indexname(indexname), _tablename(tablename), _fieldname(fieldname),
      _indextype(indextype) {}

int CreateIndexData::op() { return createindex; }

std::string CreateIndexData::IndexName() const { return _indexname; }

std::string CreateIndexData::TableName() const { return _tablename; }

std::string CreateIndexData::FieldName() const { return _fieldname; }

std::string CreateIndexData::IndexType() const { return _indextype; }
} // namespace simpledb
