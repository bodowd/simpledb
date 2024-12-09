#include "record/schema.hpp"
#include <algorithm>
#include <stdexcept>

namespace simpledb {
Schema::Schema() {}

Schema::Schema(const Schema &schema) {
  _fields = schema._fields;
  _info = schema._info;
}

Schema &Schema::operator=(const Schema &schema) {
  if (this != &schema) {
    _fields = schema._fields;
    _info = schema._info;
  }
  return *this;
}

void Schema::AddField(const std::string &fieldname, int type, int length) {
  _fields.emplace_back(fieldname);
  _info[fieldname] = Schema::FieldInfo(type, length);
}

void Schema::AddIntField(const std::string &fieldname) {
  AddField(fieldname, Schema::INTEGER, 0);
}

void Schema::AddStringField(const std::string fieldname, int length) {
  // We need to know the length of the string because it is a variable length
  // field
  AddField(fieldname, Schema::VARCHAR, length);
}

void Schema::Add(const std::string &fieldname, const Schema &schema) {
  int type = schema.Type(fieldname);
  int length = schema.Length(fieldname);
  AddField(fieldname, type, length);
}

void Schema::AddAll(const Schema &schema) {
  for (const std::string &fieldname : schema.Fields()) {
    Add(fieldname, schema);
  }
}

std::vector<std::string> Schema::Fields() const { return _fields; }

bool Schema::HasField(const std::string &fieldname) const {
  return (std::find(_fields.begin(), _fields.end(), fieldname) !=
          _fields.end());
}

int Schema::Type(const std::string &fieldname) const {
  if (_info.find(fieldname) == _info.end()) {
    throw std::runtime_error("field info " + fieldname + " not found");
  } else {
    return _info.at(fieldname).Type();
  }
}

int Schema::Length(const std::string &fieldname) const {
  if (_info.find(fieldname) == _info.end()) {
    throw std::runtime_error("field info " + fieldname + " not found");
  } else {
    return _info.at(fieldname).Length();
  }
}

Schema::FieldInfo::FieldInfo(int type, int length)
    : _type(type), _length(length) {};

int Schema::FieldInfo::Type() const { return _type; }

int Schema::FieldInfo::Length() const { return _length; }

} // namespace simpledb
