#include "morton/morton.hpp"

#include <gtest/gtest.h>

class Morton2dTest : public ::testing::Test {
 protected:
  const uint_least16_t x16_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_least16_t y16_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_least32_t m32_[64] = {
      // x = 0, 1, ..., 7
      0,  1,  4,  5,  16, 17, 20, 21,  // y = 0
      2,  3,  6,  7,  18, 19, 22, 23,  //
      8,  9,  12, 13, 24, 25, 28, 29,  //
      10, 11, 14, 15, 26, 27, 30, 31,  //
      32, 33, 36, 37, 48, 49, 52, 53,  //
      34, 35, 38, 39, 50, 51, 54, 55,  //
      40, 41, 44, 45, 56, 57, 60, 61,  //
      42, 43, 46, 47, 58, 59, 62, 63   // y = 7
  };

  const uint_least32_t x32_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_least32_t y32_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_least64_t m64_[64] = {
      // x = 0, 1, ..., 7
      0,  1,  4,  5,  16, 17, 20, 21,  // y = 0
      2,  3,  6,  7,  18, 19, 22, 23,  //
      8,  9,  12, 13, 24, 25, 28, 29,  //
      10, 11, 14, 15, 26, 27, 30, 31,  //
      32, 33, 36, 37, 48, 49, 52, 53,  //
      34, 35, 38, 39, 50, 51, 54, 55,  //
      40, 41, 44, 45, 56, 57, 60, 61,  //
      42, 43, 46, 47, 58, 59, 62, 63   // y = 7
  };
};

TEST_F(Morton2dTest, Encoding32Bit) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = morton::encode(x16_[j], y16_[i]);
      EXPECT_EQ(m, m32_[i * 8 + j])
          << "  x = " << x16_[j] << ", y = " << y16_[i] << '\n';
    }
  }
}

TEST_F(Morton2dTest, Decoding32Bit) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      uint_least16_t x, y;
      const auto k = i * 8 + j;
      morton::decode(m32_[k], x, y);
      EXPECT_EQ(x, x16_[j]) << "  m = " << m32_[k] << '\n';
      EXPECT_EQ(y, y16_[i]) << "  m = " << m32_[k] << '\n';
    }
  }
}

TEST_F(Morton2dTest, Encoding64Bit) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = morton::encode(x32_[j], y32_[i]);
      EXPECT_EQ(m, m64_[i * 8 + j])
          << "  x = " << x32_[j] << ", y = " << y32_[i] << '\n';
    }
  }
}

TEST_F(Morton2dTest, Decoding64Bit) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      uint_least32_t x, y;
      const auto k = i * 8 + j;
      morton::decode(m64_[k], x, y);
      EXPECT_EQ(x, x32_[j]) << "  m = " << m64_[k] << '\n';
      EXPECT_EQ(y, y32_[i]) << "  m = " << m64_[k] << '\n';
    }
  }
}

class Morton3dTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  const uint_least16_t x16_[4] = {0, 1, 2, 3};
  const uint_least16_t y16_[4] = {0, 1, 2, 3};
  const uint_least16_t z16_[4] = {0, 1, 2, 3};
  const uint_least32_t m32_[64] = {
      // x = 0, 1, 2, 3

      // z = 0
      0, 1, 8, 9,      // y = 0
      2, 3, 10, 11,    //
      16, 17, 24, 25,  //
      18, 19, 26, 27,  // y = 3

      // z = 1
      4, 5, 12, 13,    //
      6, 7, 14, 15,    //
      20, 21, 28, 29,  //
      22, 23, 30, 31,  //

      // z = 2
      32, 33, 40, 41,  //
      34, 35, 42, 43,  //
      48, 49, 56, 57,  //
      50, 51, 58, 59,  //

      // z = 3
      36, 37, 44, 45,  //
      38, 39, 46, 47,  //
      52, 53, 60, 61,  //
      54, 55, 62, 63   //
  };

  const uint_least32_t x32_[4] = {0, 1, 2, 3};
  const uint_least32_t y32_[4] = {0, 1, 2, 3};
  const uint_least32_t z32_[4] = {0, 1, 2, 3};
  const uint_least64_t m64_[64] = {
      // x = 0, 1, 2, 3

      // z = 0
      0, 1, 8, 9,      // y = 0
      2, 3, 10, 11,    //
      16, 17, 24, 25,  //
      18, 19, 26, 27,  // y = 3

      // z = 1
      4, 5, 12, 13,    //
      6, 7, 14, 15,    //
      20, 21, 28, 29,  //
      22, 23, 30, 31,  //

      // z = 2
      32, 33, 40, 41,  //
      34, 35, 42, 43,  //
      48, 49, 56, 57,  //
      50, 51, 58, 59,  //

      // z = 3
      36, 37, 44, 45,  //
      38, 39, 46, 47,  //
      52, 53, 60, 61,  //
      54, 55, 62, 63   //
  };
};

TEST_F(Morton3dTest, Encoding32Bit) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto m = morton::encode(x16_[k], y16_[j], z16_[i]);
        EXPECT_EQ(m, m32_[(i * 4 + j) * 4 + k])
            << "  x = " << x16_[k] << ", y = " << y16_[j] << ", z = " << z16_[i]
            << '\n';
      }
    }
  }
}

TEST_F(Morton3dTest, Decoding32Bit) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        uint_least16_t x, y, z;
        const auto l = (i * 4 + j) * 4 + k;
        morton::decode(m32_[l], x, y, z);
        EXPECT_EQ(x, x16_[k]) << "  m = " << m32_[l] << '\n';
        EXPECT_EQ(y, y16_[j]) << "  m = " << m32_[l] << '\n';
        EXPECT_EQ(z, z16_[i]) << "  m = " << m32_[l] << '\n';
      }
    }
  }
}

TEST_F(Morton3dTest, Encoding64Bit) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto m = morton::encode(x32_[k], y32_[j], z32_[i]);
        EXPECT_EQ(m, m64_[(i * 4 + j) * 4 + k])
            << "  x = " << x32_[k] << ", y = " << y32_[j] << ", z = " << z32_[i]
            << '\n';
      }
    }
  }
}

TEST_F(Morton3dTest, Decoding64Bit) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        uint_least32_t x, y, z;
        const auto l = (i * 4 + j) * 4 + k;
        morton::decode(m64_[l], x, y, z);
        EXPECT_EQ(x, x32_[k]) << "  m = " << m64_[l] << '\n';
        EXPECT_EQ(y, y32_[j]) << "  m = " << m64_[l] << '\n';
        EXPECT_EQ(z, z32_[i]) << "  m = " << m64_[l] << '\n';
      }
    }
  }
}