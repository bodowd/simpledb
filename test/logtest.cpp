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
    std::cout << "[" << s << ", " << val << "]";
  }
  std::cout << std::endl;
}

std::vector<char> createLogRecord(std::string s, int n) {
  int string_pos = 0;
  // insert the int string size after the string
  int int_pos = string_pos + Page::MaxLength(s.size());
  auto b = std::make_shared<std::vector<char>>(int_pos + sizeof(int));
  Page page(b);
  page.SetString(string_pos, s);
  page.SetInt(int_pos, n);
  return *b;
}

int createRecords(LogManager &log_manager, int start, int end) {
  std::cout << "Creating records: ";
  int lsn{0};
  for (int i = start; i <= end; i++) {
    std::vector<char> rec =
        createLogRecord("record" + std::to_string(i), i + 100);
    lsn = log_manager.Append(rec);
    std::cout << lsn << " ";
  }
  std::cout << std::endl;
  return lsn;
}

TEST(log, log_test) {
  SimpleDB db("logtest", 400, 8);
  std::unique_ptr<LogManager> lm = db.GetLogManager();
  printLogRecords(*lm, "The initial empty log file: ");
  std::cout << "End of initial log file" << std::endl;
  auto lsn1 = createRecords(*lm, 1, 35);
  ASSERT_EQ(lsn1, 35);
  printLogRecords(*lm, "\nThe log file now has these records: ");
  auto lsn2 = createRecords(*lm, 36, 70);
  ASSERT_EQ(lsn2, 70);
  std::cout << "FLUSHING ..." << std::endl;
  lm->Flush(65);
  printLogRecords(*lm, "\nThe log file now has these records: ");
}

} // namespace simpledb
