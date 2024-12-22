#pragma once
#include "metadata/tablemangager.hpp"
#include "record/layout.hpp"
#include "tx/transaction.hpp"
#include <string>
namespace simpledb {
/// pg. 193
/// A view is a table whose records are computed dynamically from a query.
/// That query is called the definition of the view.
/// The ViewManager stores view definitions in the catalog table. There is one
/// record per view.
class ViewManager {
public:
  const std::string VIEWCAT = "viewcat";

  /// Constructor is called during system startup and creates the VIEWCAT table
  /// in the catalog if the db is new.
  ViewManager(bool isNew, TableManager *tm, Transaction *tx);

  /// Insert a record into the VIEWCAT table in the catalog.
  void CreateView(const std::string &vname, const std::string &vdef,
                  Transaction *tx);

  /// Get the record corresponding to the specified view name from the catalog.
  std::string GetViewDef(const std::string &vname, Transaction *tx);

private:
  /// View definitions are stored as VARCHAR strings. The current limit to the
  /// varchar length is defined in MAX_VIEWDEF.
  const int MAX_VIEWDEF = 100;
  TableManager *_tm;
};
} // namespace simpledb
