#include "parse/createviewdata.hpp"
#include "parse/object.hpp"
namespace simpledb {
CreateViewData::CreateViewData(const std::string &viewname,
                               std::unique_ptr<QueryData> querydata)
    : _viewname(viewname), _querydata(std::move(querydata)) {}

int CreateViewData::op() { return createview; }

std::string CreateViewData::ViewName() const { return _viewname; }

std::string CreateViewData::ViewDef() const { return _querydata->ToString(); }
} // namespace simpledb
