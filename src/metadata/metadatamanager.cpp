#include "metadata/metadatamanager.hpp"
#include "metadata/tablemangager.hpp"
#include <memory>
namespace simpledb {
std::unique_ptr<TableManager> MetadataManager::_tm;
std::unique_ptr<ViewManager> MetadataManager::_vm;
std::unique_ptr<StatManager> MetadataManager::_sm;

MetadataManager::MetadataManager(bool isNew, Transaction *tx) {
  _tm = std::make_unique<TableManager>(isNew, tx);
  _vm = std::make_unique<ViewManager>(isNew, _tm.get(), tx);
  _sm = std::make_unique<StatManager>(_tm.get(), tx);
}

void MetadataManager::CreateTable(const std::string &tablename,
                                  const Schema &sch, Transaction *tx) {
  _tm->CreateTable(tablename, sch, tx);
}

Layout MetadataManager::GetLayout(const std::string &tablename,
                                  Transaction *tx) {
  return _tm->GetLayout(tablename, tx);
}

void MetadataManager::CreateView(const std::string &viewname,
                                 const std::string &viewdef, Transaction *tx) {
  _vm->CreateView(viewname, viewdef, tx);
}

std::string MetadataManager::GetViewDef(const std::string &viewname,
                                        Transaction *tx) {
  return _vm->GetViewDef(viewname, tx);
}

StatInfo MetadataManager::GetStatInfo(const std::string &tablename,
                                      const Layout &lt, Transaction *tx) {
  return _sm->GetStatInfo(tablename, lt, tx);
}

} // namespace simpledb
