#include "log/logmanager.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace simpledb {
void printLogRecords(LogManager &log_manager, const std::string msg) {
  std::cout << msg << std::endl;
  LogManager::LogIterator iterator = log_manager.Iterator();
  while (iterator.HasNext()) {
    auto rec = std::make_shared<std::vector<char>>(iterator.Next());
    Page page(rec);
    std::string s = page.GetString(0);
    int npos = Page::MaxLength(s.size());
    int val = page.GetInt(npos);
    std::cout << "[" << s << ", " << val << "]" << std::endl;
  }
  std::cout << std::endl;
}

std::vector<char> createLogRecord(std::string s, int n) {
  int npos = Page::MaxLength(s.size());
  auto b = std::make_shared<std::vector<char>>(npos + sizeof(int));
  Page page(b);
  std::cout << "createLogRecord s: " << s << std::endl;
  page.SetString(0, s);
  // std::cout << "npos: " << npos << std::endl;
  // page.SetInt(npos, n);
  return *b;
}

void createRecords(LogManager &log_manager, int start, int end) {
  std::cout << "Creating records: " << std::endl;
  for (int i = start; i <= end; i++) {
    std::vector<char> rec =
        createLogRecord("record" + std::to_string(i), i + 100);
    int lsn = log_manager.Append(rec);
    std::cout << "lsn: " << lsn << " ";
  }
  std::cout << std::endl;
}

TEST(log, log_test) {
  SimpleDB db("logtest", 400, 8);
  std::unique_ptr<LogManager> lm = db.GetLogManager();
  printLogRecords(*lm, "The initial empty log file: ");
  std::cout << "done" << std::endl;
  createRecords(*lm, 1, 35);
  printLogRecords(*lm, "The log file now has these records: ");
  // createRecords(*lm, 36, 70);
  // lm->Flush(65);
  // printLogRecords(*lm, "The log file now has these records: ");
}

} // namespace simpledb
