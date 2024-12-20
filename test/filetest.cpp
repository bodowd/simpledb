#include "server/simpledb.hpp"
#include "gtest/gtest.h"
#include <memory>
namespace simpledb {
TEST(file, file_test) {
  SimpleDB sdb("filetest", 400, 8);
  auto &fm = sdb.GetFileManager();
  BlockId blk("testfile", 2);

  int offset = 81;

  auto page = std::make_unique<Page>(fm.BlockSize());
  std::string testData = "Data for testing file";
  page->SetString(offset, testData);

  auto size = page->MaxLength(testData.size());
  int offset2 = offset + size;
  page->SetInt(offset2, 7991);
  fm.Write(blk, *page);

  auto str = page->GetString(offset);

  std::cout << "offset " << offset << " contains " << str << std::endl;
  ASSERT_EQ(str, testData);
  std::cout << "offset " << offset << " contains " << page->GetInt(offset)
            << std::endl;
  std::cout << "offset " << offset2 << " contains " << page->GetInt(offset2)
            << std::endl;
  ASSERT_EQ(page->GetInt(offset2), 7991);

  // read the written data to a new page
  std::cout << "---" << std::endl;
  std::cout << "test reading the written data into a new page: " << std::endl;
  auto page2 = std::make_unique<Page>(fm.BlockSize());
  fm.Read(blk, *page2);
  std::cout << "offset " << offset << " in page 2 contains "
            << page2->GetString(offset) << std::endl;
  std::cout << "offset " << offset2 << " in page 2 contains "
            << page2->GetInt(offset2) << std::endl;

  ASSERT_EQ(page2->GetString(offset), testData);
  ASSERT_EQ(page2->GetInt(offset2), 7991);
}
} // namespace simpledb
