#include <memory>
#include <query/productscan.hpp>
namespace simpledb {

ProductScan::ProductScan(const std::shared_ptr<Scan> s1,
                         const std::shared_ptr<Scan> s2)
    : _scan1(s1), _scan2(s2) {
  _scan1->Next();
};

void ProductScan::BeforeFirst() {
  _scan1->BeforeFirst();
  _scan1->Next();
  _scan2->BeforeFirst();
}

bool ProductScan::Next() {
  /// Each call to Next() will move to the next record of scan 2. If scan 2 has
  /// a record, it will return true. In this way, we keep scanning scan 2 until
  /// there are no more records.
  ///
  /// If there are no more records in scan 2, then that means we finished this
  /// iteration of scan 2, and will move to the next record of scan 1, and start
  /// the next nested loop scan of scan 2 again.

  /// still records in scan 2
  if (_scan2->Next()) {
    return true;
  }
  /// no more records in scan 2. move scan 2 pointer back to the beginning and
  /// increment to the next record of scan 1
  else {
    _scan2->BeforeFirst();
    return _scan2->Next() && _scan1->Next();
  }
}

int ProductScan::GetInt(const std::string &fieldname) {
  if (_scan1->HasField(fieldname)) {
    return _scan1->GetInt(fieldname);
  } else {
    return _scan2->GetInt(fieldname);
  }
}

std ::string ProductScan::GetString(const std::string &fieldname) {
  if (_scan1->HasField(fieldname)) {
    return _scan1->GetString(fieldname);
  } else {
    return _scan2->GetString(fieldname);
  }
}

Constant ProductScan::GetVal(const std::string &fieldname) {
  if (_scan1->HasField(fieldname)) {
    return _scan1->GetVal(fieldname);
  } else {
    return _scan2->GetVal(fieldname);
  }
}

bool ProductScan::HasField(const std::string &fieldname) {
  return _scan1->HasField(fieldname) || _scan2->HasField(fieldname);
}

void ProductScan::Close() {
  _scan1->Close();
  _scan2->Close();
}
} // namespace simpledb
