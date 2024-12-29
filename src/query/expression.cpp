#include "query/expression.hpp"
namespace simpledb {

Expression::Expression() {}

Expression::Expression(const Expression &e)
    : _val(e._val), _fieldname(e._fieldname) {}

Expression::Expression(const Constant &val) : _val(val) {}

Expression::Expression(const std::string &fieldname) : _fieldname(fieldname) {}

Expression &Expression::operator=(const Expression &e) {
  if (this != &e) {
    _val = e._val;
    _fieldname = e._fieldname;
  }
  return *this;
}

bool Expression::IsFieldName() const { return !_fieldname.empty(); }

Constant Expression::AsConstant() const { return _val; }

std::string Expression::AsFieldName() const { return _fieldname; }

Constant Expression::Evaluate(Scan *s) const {
  return _val.IsNull() ? s->GetVal(_fieldname) : _val;
}

bool Expression::AppliesTo(const Schema &sch) const {
  return _val.IsNull() ? sch.HasField(_fieldname) : true;
}

std::string Expression::ToString() const {
  return _val.IsNull() ? _fieldname : _val.ToString();
}

} // namespace simpledb
