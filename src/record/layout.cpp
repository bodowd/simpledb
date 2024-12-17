#include "record/layout.hpp"
#include "file/page.hpp"
#include <stdexcept>

namespace simpledb {
Layout::Layout() {}

Layout::Layout(const Layout &layout) {
  _schema = layout._schema;
  _offsets = layout._offsets;
  _slotsize = layout._slotsize;
}

Layout::Layout(const Schema &schema) {
  _schema = schema;
  // empty/in-use flag is size of int
  int pos = sizeof(int);
  for (const std::string &fieldname : _schema.Fields()) {
    _offsets[fieldname] = pos;
    pos += lengthInBytes(fieldname);
  }
  _slotsize = pos;
}

Layout::Layout(const Schema &schema, std::map<std::string, int> &offsets,
               int slotsize) {
  _schema = schema;
  _offsets = offsets;
  _slotsize = slotsize;
}

int Layout::lengthInBytes(const std::string &fieldname) {
  int fieldType = _schema.Type(fieldname);
  if (fieldType == Schema::INTEGER) {
    return sizeof(int);
  } else if (fieldType == Schema::VARCHAR) {
    return Page::MaxLength(_schema.Length(fieldname));
  } else {
    throw std::runtime_error("Field type not defined");
  }
}

Layout &Layout::operator=(const Layout &layout) {
  if (this != &layout) {
    _schema = layout._schema;
    _offsets = layout._offsets;
    _slotsize = layout._slotsize;
  }
  return *this;
}

Schema Layout::GetSchema() const { return _schema; }

int Layout::Offset(const std::string &fieldname) {
  if (_offsets.find(fieldname) == _offsets.end()) {
    throw std::runtime_error("field name " + fieldname +
                             " not found in schema");
  } else {
    return _offsets[fieldname];
  }
}

int Layout::SlotSize() const { return _slotsize; }
} // namespace simpledb
