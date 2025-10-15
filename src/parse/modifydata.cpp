#include "parse/modifydata.hpp"

namespace simpledb {
ModifyData::ModifyData(const std::string &tablename,
                       const std::string &fieldname, const Expression &newVal,
                       const Predicate &pred)
    : _tablename(tablename), _fieldname(fieldname), _newVal(newVal),
      _pred(pred) {}

int ModifyData::op() { return modify; }

std::string ModifyData::TableName() const { return _tablename; }

std::string ModifyData::TargetField() const { return _fieldname; }

Expression ModifyData::NewVal() const { return _newVal; }

Predicate ModifyData::Pred() const { return _pred; }

} // namespace simpledb
