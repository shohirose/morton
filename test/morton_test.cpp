// This software is released under the MIT license.
//
// Copyright (c) 2016 Jeroen Baert
// Copyright (c) 2020 Sho Hirose

#include "morton/morton.hpp"

#include <gtest/gtest.h>

using morton::decode;
using morton::encode;

namespace tag = morton::tag;

class Morton2d32BitTest : public ::testing::Test {
 protected:
  const uint_fast16_t x_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_fast16_t y_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_fast32_t m_[64] = {
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
      const auto m = encode(x_[j], y_[i], tag::preshifted_lookup_table{});
      EXPECT_EQ(m, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = encode(x_[j], y_[i], tag::lookup_table{});
      EXPECT_EQ(m, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      uint_fast16_t x, y;
      const auto k = i * 8 + j;
      decode(m_[k], x, y, tag::preshifted_lookup_table{});
      EXPECT_EQ(x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

TEST_F(Morton2d32BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      uint_fast16_t x, y;
      const auto k = i * 8 + j;
      decode(m_[k], x, y, tag::lookup_table{});
      EXPECT_EQ(x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

class Morton2d64BitTest : public ::testing::Test {
 protected:
  const uint_fast32_t x_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_fast32_t y_[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const uint_fast64_t m_[64] = {
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
      const auto m = encode(x_[j], y_[i], tag::preshifted_lookup_table{});
      EXPECT_EQ(m, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const auto m = encode(x_[j], y_[i], tag::lookup_table{});
      EXPECT_EQ(m, m_[i * 8 + j])
          << "  x = " << x_[j] << ", y = " << y_[i] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      uint_fast32_t x, y;
      const auto k = i * 8 + j;
      decode(m_[k], x, y, tag::preshifted_lookup_table{});
      EXPECT_EQ(x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

TEST_F(Morton2d64BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      uint_fast32_t x, y;
      const auto k = i * 8 + j;
      decode(m_[k], x, y, tag::lookup_table{});
      EXPECT_EQ(x, x_[j]) << "  m = " << m_[k] << '\n';
      EXPECT_EQ(y, y_[i]) << "  m = " << m_[k] << '\n';
    }
  }
}

class Morton3d32BitTest : public ::testing::Test {
 protected:
  const uint_fast16_t x_[4] = {0, 1, 2, 3};
  const uint_fast16_t y_[4] = {0, 1, 2, 3};
  const uint_fast16_t z_[4] = {0, 1, 2, 3};
  const uint_fast32_t m_[64] = {
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

TEST_F(Morton3d32BitTest, EncodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto m =
            encode(x_[k], y_[j], z_[i], tag::preshifted_lookup_table{});
        EXPECT_EQ(m, m_[(i * 4 + j) * 4 + k])
            << "  x = " << x_[k] << ", y = " << y_[j] << ", z = " << z_[i]
            << '\n';
      }
    }
  }
}

TEST_F(Morton3d32BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto m = encode(x_[k], y_[j], z_[i], tag::lookup_table{});
        EXPECT_EQ(m, m_[(i * 4 + j) * 4 + k])
            << "  x = " << x_[k] << ", y = " << y_[j] << ", z = " << z_[i]
            << '\n';
      }
    }
  }
}

TEST_F(Morton3d32BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        uint_fast16_t x, y, z;
        const auto l = (i * 4 + j) * 4 + k;
        decode(m_[l], x, y, z, tag::preshifted_lookup_table{});
        EXPECT_EQ(x, x_[k]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(y, y_[j]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(z, z_[i]) << "  m = " << m_[l] << '\n';
      }
    }
  }
}

TEST_F(Morton3d32BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        uint_fast16_t x, y, z;
        const auto l = (i * 4 + j) * 4 + k;
        decode(m_[l], x, y, z, tag::lookup_table{});
        EXPECT_EQ(x, x_[k]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(y, y_[j]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(z, z_[i]) << "  m = " << m_[l] << '\n';
      }
    }
  }
}

class Morton3d64BitTest : public ::testing::Test {
 protected:
  const uint_fast32_t x_[4] = {0, 1, 2, 3};
  const uint_fast32_t y_[4] = {0, 1, 2, 3};
  const uint_fast32_t z_[4] = {0, 1, 2, 3};
  const uint_fast64_t m_[64] = {
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

TEST_F(Morton3d64BitTest, EncodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto m =
            encode(x_[k], y_[j], z_[i], tag::preshifted_lookup_table{});
        EXPECT_EQ(m, m_[(i * 4 + j) * 4 + k])
            << "  x = " << x_[k] << ", y = " << y_[j] << ", z = " << z_[i]
            << '\n';
      }
    }
  }
}

TEST_F(Morton3d64BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto m = encode(x_[k], y_[j], z_[i], tag::lookup_table{});
        EXPECT_EQ(m, m_[(i * 4 + j) * 4 + k])
            << "  x = " << x_[k] << ", y = " << y_[j] << ", z = " << z_[i]
            << '\n';
      }
    }
  }
}

TEST_F(Morton3d64BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        uint_fast32_t x, y, z;
        const auto l = (i * 4 + j) * 4 + k;
        decode(m_[l], x, y, z, tag::preshifted_lookup_table{});
        EXPECT_EQ(x, x_[k]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(y, y_[j]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(z, z_[i]) << "  m = " << m_[l] << '\n';
      }
    }
  }
}

TEST_F(Morton3d64BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        uint_fast32_t x, y, z;
        const auto l = (i * 4 + j) * 4 + k;
        decode(m_[l], x, y, z, tag::lookup_table{});
        EXPECT_EQ(x, x_[k]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(y, y_[j]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(z, z_[i]) << "  m = " << m_[l] << '\n';
      }
    }
  }
}

TEST(MortonBugReportTest, IssueNo25Case) {
  // 32 Bits
  {
    const uint_fast16_t x = 1971;
    const uint_fast16_t y = 1951;
    const uint_fast16_t z = 975;
    const uint_fast32_t m_correct = 4293967295;
    EXPECT_EQ(encode(x, y, z, tag::preshifted_lookup_table{}), m_correct);
    EXPECT_EQ(encode(x, y, z, tag::lookup_table{}), m_correct);
  }

  // 64 Bits
  {
    const uint_fast32_t x = 1971;
    const uint_fast32_t y = 1951;
    const uint_fast32_t z = 975;
    const uint_fast64_t m_correct = 4293967295;
    EXPECT_EQ(encode(x, y, z, tag::preshifted_lookup_table{}), m_correct);
    EXPECT_EQ(encode(x, y, z, tag::lookup_table{}), m_correct);
  }

  // 64 Bits
  {
    const uint_fast32_t xt = 2097075;
    const uint_fast32_t yt = 2097055;
    const uint_fast32_t zt = 2097103;
    const uint_fast64_t mt = 9223372036853775807;
    EXPECT_EQ(encode(xt, yt, zt, tag::preshifted_lookup_table{}), mt);
    EXPECT_EQ(encode(xt, yt, zt, tag::lookup_table{}), mt);

    uint_fast32_t x, y, z;
    decode(mt, x, y, z, tag::preshifted_lookup_table{});
    EXPECT_EQ(x, xt);
    EXPECT_EQ(y, yt);
    EXPECT_EQ(z, zt);

    decode(mt, x, y, z, tag::lookup_table{});
    EXPECT_EQ(x, xt);
    EXPECT_EQ(y, yt);
    EXPECT_EQ(z, zt);
  }
}