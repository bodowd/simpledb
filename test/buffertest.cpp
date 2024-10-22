#include "file/blockid.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
namespace simpledb {
TEST(buffer, buffer_test) {
  SimpleDB db("buffertest", 400, 3);
  BufferManager &bm = db.GetBufferManager();

  auto buff1 = bm.Pin(BlockId("testfile", 1));
  auto page = buff1->Contents();
  auto n = page->GetInt(80);
  // this modification will get written to disk
  page->SetInt(80, n + 562);
  buff1->SetModified(1, 0);

  std::cout << "The new value is " << (n + 562) << std::endl;
  ASSERT_EQ(page->GetInt(80), 562);
  // After unpinning buff1, it is now possible for it to be evicted
  // Since there are only 3 buffers in the buffer pool,
  // if we pin three more buffers, buff1 will get replaced (and thereby
  // written to disk)
  bm.Unpin(buff1);

  // One of these pins will flush buff1 to disk
  auto buff2 = bm.Pin(BlockId("testfile", 2));
  auto buff3 = bm.Pin(BlockId("testfile", 3));
  auto buff4 = bm.Pin(BlockId("testfile", 4));

  auto buff2Contents = buff2->Contents()->GetInt(80);
  std::cout << "Contents of buff2: " << buff2Contents << std::endl;
  ASSERT_EQ(buff2Contents, 0);
  // Unpin buff2 and then we will pin block 1 back to it
  // and bring it back from disk
  bm.Unpin(buff2);
  buff2 = bm.Pin(BlockId("testfile", 1));
  auto p2 = buff2->Contents();

  auto newBuff2Contents = p2->GetInt(80);
  std::cout << "The contents of block 1 now are in buff 2: " << p2->GetInt(80)
            << std::endl;
  ASSERT_EQ(newBuff2Contents, 562);
  // This modification won't get written to disk
  p2->SetInt(80, 9999);
  buff2->SetModified(1, 0);
  bm.Unpin(buff2);

  std::cout << "Content of buff2 that will not be written to disk: "
            << buff2->Contents()->GetInt(80) << std::endl;
}
} // namespace simpledb
