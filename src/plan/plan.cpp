#include "parse/object.hpp"
#include "parse/parser.hpp"
#include "plan/planner.hpp"
#include "tx/transaction.hpp"
#include <memory>
namespace simpledb {
Planner::Planner(std::unique_ptr<QueryPlanner> qp,
                 std::unique_ptr<UpdatePlanner> up)
    : _query_planner(std::move(qp)), _update_planner(std::move(up)) {}

std::shared_ptr<Plan> Planner::CreateQueryPlan(const std::string &cmd,
                                               Transaction *tx) {
  Parser parser(cmd);
  std::unique_ptr<QueryData> data = parser.Query();
  return _query_planner->CreatePlan(data.get(), tx);
}

int Planner::ExecuteUpdate(const std::string &cmd, Transaction *tx) {
  Parser parser(cmd);
  std::unique_ptr<Object> obj = parser.UpdateCmd();
  if (obj->op() == Object::insert) {
    /// static_cast<InsertData *>(obj.get()) tells the compiler to treat the
    /// pointer returned by obj.get() to be a pointer to an InsertData object
    return _update_planner->ExecuteInsert(static_cast<InsertData *>(obj.get()),
                                          tx);
  } else if (obj->op() == Object::remove) {
    return _update_planner->ExecuteDelete(static_cast<DeleteData *>(obj.get()),
                                          tx);
  } else if (obj->op() == Object::modify) {
    return _update_planner->ExecuteModify(static_cast<ModifyData *>(obj.get()),
                                          tx);
  } else if (obj->op() == Object::createtable) {
    return _update_planner->ExecuteCreateTable(
        static_cast<CreateTableData *>(obj.get()), tx);
  } else if (obj->op() == Object::createview) {
    return _update_planner->ExecuteCreateView(
        static_cast<CreateViewData *>(obj.get()), tx);
  } else if (obj->op() == Object::createindex) {
    return _update_planner->ExecuteCreateIndex(
        static_cast<CreateIndexData *>(obj.get()), tx);
  } else {
    return 0; // this should never occur
  }
}
} // namespace simpledb
