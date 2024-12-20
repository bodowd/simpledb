#include "metadata/statmanager.hpp"
#include "metadata/tablemangager.hpp"
#include "metadata/viewmanager.hpp"
#include "tx/transaction.hpp"

namespace simpledb {
/// Single place for clients to obtain metadata.
class MetadataManager {
private:
  static std::unique_ptr<TableManager> _tm;
  static std::unique_ptr<ViewManager> _vm;
  static std::unique_ptr<StatManager> _sm;

public:
  MetadataManager(bool isNew, Transaction *tx);
  void CreateTable(const std::string &tablename, const Schema &sch,
                   Transaction *tx);
  Layout GetLayout(const std::string &tablename, Transaction *tx);
  void CreateView(const std::string &viewname, const std::string &viewdef,
                  Transaction *tx);
  std::string GetViewDef(const std::string &viewname, Transaction *tx);

  StatInfo GetStatInfo(const std::string &tablename, const Layout &lt,
                       Transaction *tx);
};
} // namespace simpledb
