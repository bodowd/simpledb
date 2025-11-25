#include "parse/parser.hpp"

namespace simpledb {
Parser::Parser(const std::string &s) : _lex(s) {}

std::string Parser::NewField() { return _lex.EatId(); }

Constant Parser::NewConstant() {
  if (_lex.MatchStringConstant()) {
    return Constant(_lex.EatStringConstant());
  } else {
    return Constant(_lex.EatIntConstant());
  }
}

Expression Parser::NewExpression() {
  if (_lex.MatchId()) {
    return Expression(NewField());
  } else {
    return Expression(NewConstant());
  }
}

Term Parser::NewTerm() {
  Expression left_hand_side = NewExpression();
  _lex.EatDelim('=');
  Expression right_hand_side = NewExpression();
  return Term(left_hand_side, right_hand_side);
}

/// A Recursive-Descent Parser is implemented in simpledb
/// NewPredicate illustrates how a recursive rule is implemented.
/// It keeps going through the AND predicates until there are no more "AND"
Predicate Parser::NewPredictate() {
  Predicate pred(NewTerm());
  if (_lex.MatchKeyword("and")) {
    _lex.EatKeyword("and");
    pred.ConjoinWith(NewPredictate());
  }
  return pred;
}

std::unique_ptr<QueryData> Parser::Query() {
  _lex.EatKeyword("select");
  std::vector<std::string> fields = selectList();
  _lex.EatKeyword("from");
  std::set<std::string> tables = tableList();
  Predicate pred;
  if (_lex.MatchKeyword("where")) {
    _lex.EatKeyword("where");
    pred = NewPredictate();
  }
  return std::make_unique<QueryData>(fields, tables, pred);
}

std::vector<std::string> Parser::selectList() {
  std::vector<std::string> L;
  L.emplace_back(NewField());
  if (_lex.MatchDelim(',')) {
    _lex.EatDelim(',');
    std::vector<std::string> LL = selectList();
    L.insert(L.end(), LL.begin(), LL.end());
  }
  return L;
}

std::set<std::string> Parser::tableList() {
  std::set<std::string> L;
  L.insert(_lex.EatId());
  if (_lex.MatchDelim(',')) {
    _lex.EatDelim(',');
    std::set<std::string> LL = tableList();
    L.insert(LL.begin(), LL.end());
  }
  return L;
}

std::unique_ptr<Object> Parser::UpdateCmd() {
  if (_lex.MatchKeyword("insert")) {
    return Insert();
  } else if (_lex.MatchKeyword("delete")) {
    return Delete();
  } else if (_lex.MatchKeyword("update")) {
    return Modify();
  } else {
    return create();
  }
}

std::unique_ptr<Object> Parser::create() {
  _lex.EatKeyword("create");
  if (_lex.MatchKeyword("table")) {
    return static_unique_pointer_cast<Object>(CreateTable());
  } else if (_lex.MatchKeyword("view")) {
    return static_unique_pointer_cast<Object>(CreateView());
  } else {
    return static_unique_pointer_cast<Object>(CreateIndex());
  }
}

std::unique_ptr<DeleteData> Parser::Delete() {
  _lex.EatKeyword("delete");
  _lex.EatKeyword("from");
  std::string tablename = _lex.EatId();
  Predicate pred;
  if (_lex.MatchKeyword("where")) {
    _lex.EatKeyword("where");
    pred = NewPredictate();
  }
  return std::make_unique<DeleteData>(tablename, pred);
}

std::unique_ptr<InsertData> Parser::Insert() {
  _lex.EatKeyword("insert");
  _lex.EatKeyword("into");
  std::string tablename = _lex.EatId();
  _lex.EatDelim('(');
  std::vector<std::string> fields = fieldList();
  _lex.EatDelim(')');
  _lex.EatKeyword("values");
  _lex.EatDelim('(');
  std::vector<Constant> vals = constList();
  _lex.EatDelim(')');
  return std::make_unique<InsertData>(tablename, fields, vals);
}

std::vector<std::string> Parser::fieldList() {
  std::vector<std::string> L;
  L.emplace_back(NewField());
  if (_lex.MatchDelim(',')) {
    _lex.EatDelim(',');
    std::vector<std::string> LL = fieldList();
    L.insert(L.end(), LL.begin(), LL.end());
  }
  return L;
}

std::vector<Constant> Parser::constList() {
  std::vector<Constant> L;
  L.emplace_back(NewConstant());
  if (_lex.MatchDelim(',')) {
    _lex.EatDelim(',');
    std::vector<Constant> LL = constList();
    L.insert(L.end(), LL.begin(), LL.end());
  }
  return L;
}

std::unique_ptr<ModifyData> Parser::Modify() {
  _lex.EatKeyword("update");
  std::string tablename = _lex.EatId();
  _lex.EatKeyword("set");
  std::string fieldname = NewField();
  _lex.EatDelim('=');
  Expression newVal = NewExpression();
  Predicate pred;
  if (_lex.MatchKeyword("where")) {
    _lex.EatKeyword("where");
    pred = NewPredictate();
  }
  return std::make_unique<ModifyData>(tablename, fieldname, newVal, pred);
}

Schema Parser::fieldDefs() {
  Schema schema = fieldDef();
  if (_lex.MatchDelim(',')) {
    _lex.EatDelim(',');
    Schema schema2 = fieldDefs();
    schema.AddAll(schema2);
  }
  return schema;
}

Schema Parser::fieldDef() {
  std::string fieldname = NewField();
  return fieldType(fieldname);
}

Schema Parser::fieldType(const std::string &fieldname) {
  Schema schema;
  if (_lex.MatchKeyword("int")) {
    _lex.EatKeyword("int");
    schema.AddIntField(fieldname);
  } else {
    _lex.EatKeyword("varchar");
    _lex.EatDelim('(');
    int strLen = _lex.EatIntConstant();
    _lex.EatDelim(')');
    schema.AddStringField(fieldname, strLen);
  }
  return schema;
}

std::unique_ptr<CreateTableData> Parser::CreateTable() {
  _lex.EatKeyword("table");
  std::string tablename = _lex.EatId();
  /// next come the column names and types, this will form the schema of the
  /// table
  _lex.EatDelim('(');
  Schema sch = fieldDefs();
  _lex.EatDelim(')');
  return std::make_unique<CreateTableData>(tablename, sch);
}

std::unique_ptr<CreateViewData> Parser::CreateView() {
  _lex.EatKeyword("view");
  std::string viewname = _lex.EatId();
  _lex.EatKeyword("as");
  std::unique_ptr<QueryData> q = Query();
  return std::make_unique<CreateViewData>(viewname, std::move(q));
}

std::unique_ptr<CreateIndexData> Parser::CreateIndex() {
  _lex.EatKeyword("index");
  std::string idxname = _lex.EatId();
  _lex.EatKeyword("on");
  std::string tablename = _lex.EatId();
  _lex.EatDelim('(');
  std::string fieldname = NewField();
  _lex.EatDelim(')');
  return std::make_unique<CreateIndexData>(idxname, tablename, fieldname,
                                           "btree");
}

} // namespace simpledb
