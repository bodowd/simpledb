#pragma once

#include "record/schema.hpp"
namespace simpledb {
class EmbeddedMetaData {
public:
  EmbeddedMetaData(const Schema &sch);
  unsigned int GetColumnCount();
  std::string GetColumnName(unsigned int column);
  int GetColumnType(unsigned int column);
  unsigned int GetColumnDisplaySize(unsigned int column);

private:
  Schema _schema;
};
} // namespace simpledb
