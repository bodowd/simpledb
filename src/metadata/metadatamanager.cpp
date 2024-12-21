#include "metadata/metadatamanager.hpp"
#include "metadata/tablemangager.hpp"
#include <iostream>
#include <memory>
namespace simpledb {
std::unique_ptr<TableManager> MetadataManager::_tm;
std::unique_ptr<ViewManager> MetadataManager::_vm;
std::unique_ptr<StatManager> MetadataManager::_sm;
std::unique_ptr<IndexManager> MetadataManager::_im;

MetadataManager::MetadataManager(bool isNew, Transaction *tx) {
  _tm = std::make_unique<TableManager>(isNew, tx);
  _vm = std::make_unique<ViewManager>(isNew, _tm.get(), tx);
  _sm = std::make_unique<StatManager>(_tm.get(), tx);
  _im = std::make_unique<IndexManager>(isNew, _tm.get(), _sm.get(), tx);
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

void MetadataManager::CreateIndex(const std::string &idxname,
                                  const std::string &tablename,
                                  const std::string &fieldname,
                                  Transaction *tx) {
  _im->CreateIndex(idxname, tablename, fieldname, tx);
}

std::map<std::string, IndexInfo>
MetadataManager::GetIndexInfo(const std::string &tablename, Transaction *tx) {
  return _im->GetIndexInfo(tablename, tx);
}

} // namespace simpledb
