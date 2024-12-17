#pragma once
#include "record/schema.hpp"
#include <map>

namespace simpledb {
// Contains the physical information about a record. It calculates the field and
// slot sizes, and the field offsets within the slot.
// p. 171 in the book
class Layout {
public:
  Layout();
  Layout(const Layout &layout);
  // Called when a table is created. Calculates the layout information based
  // on the given schema
  Layout(const Schema &schema);
  // Called after the table has been created. Client provides previously
  // calculated values
  Layout(const Schema &schema, std::map<std::string, int> &offsets,
         int slotsize);
  Schema GetSchema() const;
  int Offset(const std::string &fieldname);
  int SlotSize() const;
  Layout &operator=(const Layout &layout);

private:
  Schema _schema;
  std::map<std::string, int> _offsets;
  int _slotsize;
  int lengthInBytes(const std::string &fieldname);
};
} // namespace simpledb
