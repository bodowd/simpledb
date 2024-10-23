#include "file/blockid.hpp"
#include "server/simpledb.hpp"
#include "gtest/gtest.h"
namespace simpledb {
TEST(buffermanager, buffer_manager_test) {
  SimpleDB db("buffermanagertest", 400, 3);

  BufferManager &bm = db.GetBufferManager();
  bm.SetMaxTime(100);

  std::vector<Buffer *> buff(6);
  buff[0] = bm.Pin(BlockId("testfile", 0));
  buff[1] = bm.Pin(BlockId("testfile", 1));
  buff[2] = bm.Pin(BlockId("testfile", 2));

  bm.Unpin(buff[1]);
  buff[1] = nullptr;

  buff[3] = bm.Pin(BlockId("testfile", 0));
  buff[4] = bm.Pin(BlockId("testfile", 1));

  std::cout << "Available buffers: " << bm.Available() << std::endl;
  ASSERT_EQ(bm.Available(), 0);
  // buffers are all pinned, so we cannot pin block 3 to another buffer
  try {
    std::cout << "Attempting to pin block 3" << std::endl;
    buff[5] = bm.Pin(BlockId("testfile", 3));
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    ASSERT_STREQ(e.what(), "BufferAbortException");
  }
  // unpin a buffer to free up a buffer to pin block 3
  bm.Unpin(buff[2]);
  buff[2] = nullptr;
  ASSERT_EQ(bm.Available(), 1);
  std::cout << "Now pinning block 3 should work" << std::endl;
  buff[5] = bm.Pin(BlockId("testfile", 3));
  ASSERT_EQ(bm.Available(), 0);

  std::cout << "Final Buffer Allocation: " << std::endl;
  for (int i = 0; i < buff.size(); i++) {
    Buffer *b = buff[i];
    if (b != nullptr) {
      std::cout << "buff[" << i << "] pinned to block " << b->Block().ToString()
                << std::endl;
    }
  }

  // buff[0] and buff[3] should point to the same buffer because buff[0] already
  // pinned block 0 and the buffer manager should identify that an existing
  // buffer already pinned block 0 and return that buffer
  std::cout << "buff[0] and buff[3] should point to the same buffer: "
            << buff[0] << " " << buff[3] << std::endl;
  ASSERT_EQ(buff[0], buff[3]);
}
} // namespace simpledb
