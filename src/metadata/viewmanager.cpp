#include "metadata/viewmanager.hpp"
#include "metadata/tablemangager.hpp"
#include "record/tablescan.hpp"
namespace simpledb {

ViewManager::ViewManager(bool isNew, TableManager *tm, Transaction *tx) {
  _tm = tm;
  if (isNew) {
    Schema sch;
    sch.AddStringField("viewname", _tm->MAX_NAME);
    sch.AddStringField("viewdef", MAX_VIEWDEF);
    _tm->CreateTable(VIEWCAT, sch, tx);
  }
}

void ViewManager::CreateView(const std::string &vname, const std::string &vdef,
                             Transaction *tx) {
  Layout layout = _tm->GetLayout(VIEWCAT, tx);
  TableScan ts = TableScan(tx, VIEWCAT, layout);
  ts.Insert();
  ts.SetString("viewname", vname);
  ts.SetString("viewdef", vdef);
  ts.Close();
}

std::string ViewManager::GetViewDef(const std::string &vname, Transaction *tx) {
  std::string result;
  Layout layout = _tm->GetLayout(VIEWCAT, tx);
  TableScan ts = TableScan(tx, VIEWCAT, layout);
  while (ts.Next()) {
    if (ts.GetString("viewname") == vname) {
      result = ts.GetString("viewdef");
      break;
    }
  }
  ts.Close();
  return result;
}

} // namespace simpledb
