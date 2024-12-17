#pragma once
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
namespace simpledb {
// Schema holds a record's schema.
// It stores triples of the form [fieldname, type, length]
class Schema {
public:
  enum type { INTEGER = 0, VARCHAR = 1 };

  Schema();

  Schema(const Schema &schema);

  Schema &operator=(const Schema &schema);

  void AddField(const std::string &fieldname, int type, int length);

  void AddIntField(const std::string &fieldname);

  void AddStringField(const std::string fieldname, int length);

  void Add(const std::string &fieldname, const Schema &schema);

  void AddAll(const Schema &schema);

  std::vector<std::string> Fields() const;

  bool HasField(const std::string &fieldname) const;

  int Type(const std::string &fieldname) const;

  int Length(const std::string &fieldname) const;

  class FieldInfo {
  private:
    int _type;
    int _length;

  public:
    FieldInfo() = default;
    FieldInfo(int type, int length);
    int Type() const;
    int Length() const;
  };

private:
  // The fieldnames are stored in this vector
  std::vector<std::string> _fields;
  // These [fieldname, type, length] triples are stored in the _info map
  std::map<std::string, FieldInfo> _info;
};
} // namespace simpledb
