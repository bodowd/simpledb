#include "jdbc/embeddedmetadata.hpp"
#include "record/schema.hpp"
namespace simpledb {
EmbeddedMetaData::EmbeddedMetaData(const Schema &sch) : _schema(sch) {}

unsigned int EmbeddedMetaData::GetColumnCount() {
  return _schema.Fields().size();
}

std::string EmbeddedMetaData::GetColumnName(unsigned int column) {
  return _schema.Fields()[column - 1];
}

int EmbeddedMetaData::GetColumnType(unsigned int column) {
  auto fieldname = GetColumnName(column);
  return _schema.Type(fieldname);
}

unsigned int EmbeddedMetaData::GetColumnDisplaySize(unsigned int column) {
  std::string fieldname = GetColumnName(column);
  int fieldtype = _schema.Type(fieldname);
  int fieldlength =
      (fieldtype == Schema::INTEGER) ? 6 : _schema.Length(fieldname);
  return std::max(static_cast<int>(fieldname.size()), fieldlength) + 1;
}
} // namespace simpledb
