// This software is released under the MIT license.
//
// Copyright (c) 2016 Jeroen Baert
// Copyright (c) 2020 Sho Hirose

#include "morton/morton2d.hpp"

#include <gtest/gtest.h>

using namespace morton2d;

class Morton2d32BitTest : public ::testing::Test {
 protected:
  const uint16_t x_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint16_t y_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint32_t m_[64] = {
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

TEST_F(Morton2d32BitTest, EncodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m =
          encode(coordinates16_t{x_[j], y_[i]}, tag::preshifted_lookup_table{});
      EXPECT_EQ(m.value, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = encode(coordinates16_t{x_[j], y_[i]}, tag::lookup_table{});
      EXPECT_EQ(m.value, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, EncodingUsingMagicBits) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = encode(coordinates16_t{x_[j], y_[i]}, tag::magic_bits{});
      EXPECT_EQ(m.value, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto k = i * 8 + j;
      const auto c =
          decode(morton_code32_t{m_[k]}, tag::preshifted_lookup_table{});
      EXPECT_EQ(c.x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(c.y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto k = i * 8 + j;
      const auto c = decode(morton_code32_t{m_[k]}, tag::lookup_table{});
      EXPECT_EQ(c.x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(c.y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, DecodingUsingMagicBits) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto k = i * 8 + j;
      const auto c = decode(morton_code32_t{m_[k]}, tag::magic_bits{});
      EXPECT_EQ(c.x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(c.y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

class Morton2d64BitTest : public ::testing::Test {
 protected:
  const uint32_t x_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint32_t y_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint64_t m_[64] = {
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

TEST_F(Morton2d64BitTest, EncodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m =
          encode(coordinates32_t{x_[j], y_[i]}, tag::preshifted_lookup_table{});
      EXPECT_EQ(m.value, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = encode(coordinates32_t{x_[j], y_[i]}, tag::lookup_table{});
      EXPECT_EQ(m.value, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, EncodingUsingMagicBits) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = encode(coordinates32_t{x_[j], y_[i]}, tag::magic_bits{});
      EXPECT_EQ(m.value, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto k = i * 8 + j;
      const auto c =
          decode(morton_code64_t{m_[k]}, tag::preshifted_lookup_table{});
      EXPECT_EQ(c.x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(c.y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto k = i * 8 + j;
      const auto c = decode(morton_code64_t{m_[k]}, tag::lookup_table{});
      EXPECT_EQ(c.x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(c.y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, DecodingUsingMagicBits) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto k = i * 8 + j;
      const auto c = decode(morton_code64_t{m_[k]}, tag::magic_bits{});
      EXPECT_EQ(c.x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(c.y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}