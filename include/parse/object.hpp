#pragma once
#include <ostream>
namespace simpledb {
class Object {
public:
  enum Operation {
    insert,
    remove, /// delete is a c++ keyword
    modify,
    createtable,
    createview,
    createindex
  };
  virtual ~Object() {};
  virtual int op() = 0;
};

/// Override stream insertion operator "<<" in order to nicely print out the
/// enum
inline std::ostream &operator<<(std::ostream &os, const Object::Operation &op) {
  switch (op) {
  case Object::insert:
    os << "insert";
    break;
  case Object::remove:
    os << "remove";
    break;
  case Object::modify:
    os << "modify";
    break;
  case Object::createtable:
    os << "createtable";
    break;
  case Object::createview:
    os << "createview";
    break;
  case Object::createindex:
    os << "createindex";
    break;
  default:
    break;
  }
  return os;
}
} // namespace simpledb
