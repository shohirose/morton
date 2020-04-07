// This software is released under the MIT license.
//
// Copyright (c) 2016 Jeroen Baert
// Copyright (c) 2020 Sho Hirose

#include "morton/morton3d.hpp"

#include <gtest/gtest.h>

using namespace morton3d;

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
        const auto m = encode(coordinates16_t{x_[k], y_[j], z_[i]},
                              tag::preshifted_lookup_table{});
        EXPECT_EQ(m.value, m_[(i * 4 + j) * 4 + k])
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
        const auto m =
            encode(coordinates32_t{x_[k], y_[j], z_[i]}, tag::lookup_table{});
        EXPECT_EQ(m.value, m_[(i * 4 + j) * 4 + k])
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
        const auto l = (i * 4 + j) * 4 + k;
        const auto c =
            decode(morton_code32_t{m_[l]}, tag::preshifted_lookup_table{});
        EXPECT_EQ(c.x, x_[k]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(c.y, y_[j]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(c.z, z_[i]) << "  m = " << m_[l] << '\n';
      }
    }
  }
}

TEST_F(Morton3d32BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto l = (i * 4 + j) * 4 + k;
        const auto c = decode(morton_code32_t{m_[l]}, tag::lookup_table{});
        EXPECT_EQ(c.x, x_[k]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(c.y, y_[j]) << "  m = " << m_[l] << '\n';
        EXPECT_EQ(c.z, z_[i]) << "  m = " << m_[l] << '\n';
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
        const coordinates32_t c{x_[k], y_[j], z_[i]};
        const auto m = encode(c, tag::preshifted_lookup_table{});
        EXPECT_EQ(m.value, m_[(i * 4 + j) * 4 + k])
            << "  coordinates: " << c << '\n';
      }
    }
  }
}

TEST_F(Morton3d64BitTest, EncodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const coordinates32_t c{x_[k], y_[j], z_[i]};
        const auto m = encode(c, tag::lookup_table{});
        EXPECT_EQ(m.value, m_[(i * 4 + j) * 4 + k])
            << "  coordinates: " << c << '\n';
      }
    }
  }
}

TEST_F(Morton3d64BitTest, DecodingUsingPreshiftedLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto l = (i * 4 + j) * 4 + k;
        const morton_code64_t m{m_[l]};
        const coordinates32_t ct{x_[k], y_[j], z_[i]};
        const auto c = decode(m, tag::preshifted_lookup_table{});
        EXPECT_EQ(ct, c) << "  morton code: " << m
                         << "\n  correct coordinates: " << ct
                         << "\n  decoded coordinates: " << c << '\n';
      }
    }
  }
}

TEST_F(Morton3d64BitTest, DecodingUsingLookupTable) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        const auto l = (i * 4 + j) * 4 + k;
        const morton_code64_t m{m_[l]};
        const coordinates32_t ct{x_[k], y_[j], z_[i]};
        const auto c = decode(m, tag::lookup_table{});
        EXPECT_EQ(ct, c) << "  morton code: " << m
                         << "\n  correct coordinates: " << ct
                         << "\n  decoded coordinates: " << c << '\n';
      }
    }
  }
}

TEST(MortonBugReportTest, IssueNo25Case) {
  // 32 Bits
  {
    const coordinates16_t c{1971, 1951, 975};
    const morton_code32_t m{4293967295};
    EXPECT_EQ(encode(c, tag::preshifted_lookup_table{}), m);
    EXPECT_EQ(encode(c, tag::lookup_table{}), m);
#ifdef MORTON3D_USE_BMI
    EXPECT_EQ(encode(c, tag::bmi{}), m);
#endif  // MORTON3D_USE_BMI
  }

  // 64 Bits
  {
    const coordinates32_t c{1971, 1951, 975};
    const morton_code64_t m{4293967295};
    EXPECT_EQ(encode(c, tag::preshifted_lookup_table{}), m);
    EXPECT_EQ(encode(c, tag::lookup_table{}), m);
#ifdef MORTON3D_USE_BMI
    EXPECT_EQ(encode(c, tag::bmi{}), m);
#endif  // MORTON3D_USE_BMI
  }

  // 64 Bits
  {
    const coordinates32_t c{2097075, 2097055, 2097103};
    const morton_code64_t m{9223372036853775807};
    EXPECT_EQ(encode(c, tag::preshifted_lookup_table{}), m);
    EXPECT_EQ(encode(c, tag::lookup_table{}), m);
    EXPECT_EQ(decode(m, tag::preshifted_lookup_table{}), c);
    EXPECT_EQ(decode(m, tag::lookup_table{}), c);
#ifdef MORTON3D_USE_BMI
    EXPECT_EQ(encode(c, tag::bmi{}), m);
    EXPECT_EQ(decode(m, tag::bmi{}), c);
#endif  // MORTON3D_USE_BMI
  }
}