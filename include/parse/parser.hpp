#pragma once
#include "parse/createindexdata.hpp"
#include "parse/createtabledata.hpp"
#include "parse/createviewdata.hpp"
#include "parse/deletedata.hpp"
#include "parse/insertdata.hpp"
#include "parse/lexer.hpp"
#include "parse/modifydata.hpp"
#include "parse/object.hpp"
#include "parse/querydata.hpp"
#include "query/expression.hpp"
#include "query/predicate.hpp"
#include "record/schema.hpp"
#include <memory>
#include <string>
#include <vector>
namespace simpledb {

struct SelectList {
  /// These fields have no aggregation function associated with them
  std::vector<std::string> fields;
  /// These fields are to be summed
  std::vector<std::string> sum_fields;
};

class Parser {
public:
  Parser(const std::string &s);
  /// These methods will use the Eat functions from the lexer. By eating the
  /// token it will get the information about the token in that way
  ///
  std::string NewField(); /// Identifiers
  /// Like int 123 or string 'ABC' constants
  Constant NewConstant();
  Expression NewExpression();
  Term NewTerm();
  Predicate NewPredictate();

  std::unique_ptr<QueryData> Query();
  std::unique_ptr<Object> UpdateCmd();
  std::unique_ptr<InsertData> Insert();
  std::unique_ptr<DeleteData> Delete();
  std::unique_ptr<ModifyData> Modify();

  std::unique_ptr<CreateTableData> CreateTable();
  std::unique_ptr<CreateViewData> CreateView();
  std::unique_ptr<CreateIndexData> CreateIndex();

private:
  Lexer _lex;

  /// The fields that are in the "SELECT" statement
  /// i.e. SELECT a, b FROM ...
  /// a, b are added to this list
  SelectList selectList();
  /// All the tables in the "FROM" portion of the SELECT statement
  std::set<std::string> tableList();

  /// Create a table, view, or Index depending on the eaten keyword from the
  /// lexer
  std::unique_ptr<Object> create();

  /// The fields in the insert statement
  /// SimpleDB uses INSERT INTO tablename (field1, field2, ...) VALUES (value1,
  /// value2, ...)
  std::vector<std::string> fieldList();
  /// The values in the INSERT statement
  std::vector<Constant> constList();

  /// These functions create a schema defined by the create table statement
  /// by getting the field definitions from the statement
  /// For example, create table users (id int, name varchar(30))
  Schema fieldDefs();
  /// Add the name of the field to the schema
  Schema fieldDef();
  /// Add the type of the field to the schema of the table.
  /// This is specified in create table statement simpledb supports only int and
  /// varchar
  Schema fieldType(const std::string &fieldname);

  /// change the type of the object held by the unique_ptr
  template <typename TO, typename FROM>
  std::unique_ptr<TO> static_unique_pointer_cast(std::unique_ptr<FROM> &&old) {
    return std::unique_ptr<TO>{static_cast<TO *>(old.release())};
    // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
  }
};
} // namespace simpledb
