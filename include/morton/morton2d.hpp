// This software is released under the MIT license.
//
// Copyright (c) 2016 Jeroen Baert
// Copyright (c) 2020 Sho Hirose
#ifndef MORTON_MORTON2D_HPP
#define MORTON_MORTON2D_HPP

#if defined(__BMI2__) || __AVX2__
#define MORTON2D_USE_BMI
#include <immintrin.h>
#endif

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace morton2d {

/// Tags for partial specialization of morton_impl and morton3d classes.
namespace tag {

/// Tag for the implementation using pre-shifted look-up table
struct preshifted_lookup_table {};

/// Tag for the implementation using look-up table
struct lookup_table {};

/// Tag for the implementation using BMI instructions
struct bmi {};

}  // namespace tag

/// Check if a given type is a tag.
/// @tparam Tag Tag type
template <typename Tag>
struct is_tag : std::conditional<
                    (std::is_same<Tag, tag::bmi>::value ||
                     std::is_same<Tag, tag::preshifted_lookup_table>::value ||
                     std::is_same<Tag, tag::lookup_table>::value),
                    std::true_type, std::false_type>::type {};

/// @brief Morton code
/// @tparam T Value type
template <typename T>
struct morton_code {
  using value_type = T;

  T value;
};

/// Morton code in 32 bits
using morton_code32_t = morton_code<uint_fast32_t>;
/// Morton code in 64 bits
using morton_code64_t = morton_code<uint_fast64_t>;

template <typename T>
bool operator==(morton_code<T> m1, morton_code<T> m2) noexcept {
  return m1.value == m2.value;
}

template <typename T>
bool operator!=(morton_code<T> m1, morton_code<T> m2) noexcept {
  return m1.value != m2.value;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const morton_code<T> m) {
  return os << m.value;
}

template <typename T>
std::istream& operator>>(std::istream& is, morton_code<T>& m) {
  return is >> m.value;
}

/// @brief Coordinates
/// @tparam T Value type
template <typename T>
struct coordinates {
  using value_type = T;

  T x;  /// X coordinate
  T y;  /// Y coordinate
};

/// Coordinates in 16 bits
using coordinates16_t = coordinates<uint_least16_t>;
/// Coordinates in 32 bits
using coordinates32_t = coordinates<uint_least32_t>;

template <typename T>
bool operator==(const coordinates<T>& c1, const coordinates<T>& c2) noexcept {
  return c1.x == c2.x && c1.y == c2.y;
}

template <typename T>
bool operator!=(const coordinates<T>& c1, const coordinates<T>& c2) noexcept {
  return !(c1 == c2);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const coordinates<T>& c) {
  return os << "(" << c.x << ", " << c.y << ")";
}

template <typename T>
std::istream& operator>>(std::istream& is, coordinates<T>& c) {
  return is >> c.x >> c.y;
}

/// Detail implementation of morton library
namespace detail {

/// Look-up tables
namespace lookup_table {

/// For encoding
namespace encode {

static const uint_fast16_t x[256] = {
    0,     1,     4,     5,     16,    17,    20,    21,    64,    65,    68,
    69,    80,    81,    84,    85,    256,   257,   260,   261,   272,   273,
    276,   277,   320,   321,   324,   325,   336,   337,   340,   341,   1024,
    1025,  1028,  1029,  1040,  1041,  1044,  1045,  1088,  1089,  1092,  1093,
    1104,  1105,  1108,  1109,  1280,  1281,  1284,  1285,  1296,  1297,  1300,
    1301,  1344,  1345,  1348,  1349,  1360,  1361,  1364,  1365,  4096,  4097,
    4100,  4101,  4112,  4113,  4116,  4117,  4160,  4161,  4164,  4165,  4176,
    4177,  4180,  4181,  4352,  4353,  4356,  4357,  4368,  4369,  4372,  4373,
    4416,  4417,  4420,  4421,  4432,  4433,  4436,  4437,  5120,  5121,  5124,
    5125,  5136,  5137,  5140,  5141,  5184,  5185,  5188,  5189,  5200,  5201,
    5204,  5205,  5376,  5377,  5380,  5381,  5392,  5393,  5396,  5397,  5440,
    5441,  5444,  5445,  5456,  5457,  5460,  5461,  16384, 16385, 16388, 16389,
    16400, 16401, 16404, 16405, 16448, 16449, 16452, 16453, 16464, 16465, 16468,
    16469, 16640, 16641, 16644, 16645, 16656, 16657, 16660, 16661, 16704, 16705,
    16708, 16709, 16720, 16721, 16724, 16725, 17408, 17409, 17412, 17413, 17424,
    17425, 17428, 17429, 17472, 17473, 17476, 17477, 17488, 17489, 17492, 17493,
    17664, 17665, 17668, 17669, 17680, 17681, 17684, 17685, 17728, 17729, 17732,
    17733, 17744, 17745, 17748, 17749, 20480, 20481, 20484, 20485, 20496, 20497,
    20500, 20501, 20544, 20545, 20548, 20549, 20560, 20561, 20564, 20565, 20736,
    20737, 20740, 20741, 20752, 20753, 20756, 20757, 20800, 20801, 20804, 20805,
    20816, 20817, 20820, 20821, 21504, 21505, 21508, 21509, 21520, 21521, 21524,
    21525, 21568, 21569, 21572, 21573, 21584, 21585, 21588, 21589, 21760, 21761,
    21764, 21765, 21776, 21777, 21780, 21781, 21824, 21825, 21828, 21829, 21840,
    21841, 21844, 21845};

static const uint_fast16_t y[256] = {
    0,     2,     8,     10,    32,    34,    40,    42,    128,   130,   136,
    138,   160,   162,   168,   170,   512,   514,   520,   522,   544,   546,
    552,   554,   640,   642,   648,   650,   672,   674,   680,   682,   2048,
    2050,  2056,  2058,  2080,  2082,  2088,  2090,  2176,  2178,  2184,  2186,
    2208,  2210,  2216,  2218,  2560,  2562,  2568,  2570,  2592,  2594,  2600,
    2602,  2688,  2690,  2696,  2698,  2720,  2722,  2728,  2730,  8192,  8194,
    8200,  8202,  8224,  8226,  8232,  8234,  8320,  8322,  8328,  8330,  8352,
    8354,  8360,  8362,  8704,  8706,  8712,  8714,  8736,  8738,  8744,  8746,
    8832,  8834,  8840,  8842,  8864,  8866,  8872,  8874,  10240, 10242, 10248,
    10250, 10272, 10274, 10280, 10282, 10368, 10370, 10376, 10378, 10400, 10402,
    10408, 10410, 10752, 10754, 10760, 10762, 10784, 10786, 10792, 10794, 10880,
    10882, 10888, 10890, 10912, 10914, 10920, 10922, 32768, 32770, 32776, 32778,
    32800, 32802, 32808, 32810, 32896, 32898, 32904, 32906, 32928, 32930, 32936,
    32938, 33280, 33282, 33288, 33290, 33312, 33314, 33320, 33322, 33408, 33410,
    33416, 33418, 33440, 33442, 33448, 33450, 34816, 34818, 34824, 34826, 34848,
    34850, 34856, 34858, 34944, 34946, 34952, 34954, 34976, 34978, 34984, 34986,
    35328, 35330, 35336, 35338, 35360, 35362, 35368, 35370, 35456, 35458, 35464,
    35466, 35488, 35490, 35496, 35498, 40960, 40962, 40968, 40970, 40992, 40994,
    41000, 41002, 41088, 41090, 41096, 41098, 41120, 41122, 41128, 41130, 41472,
    41474, 41480, 41482, 41504, 41506, 41512, 41514, 41600, 41602, 41608, 41610,
    41632, 41634, 41640, 41642, 43008, 43010, 43016, 43018, 43040, 43042, 43048,
    43050, 43136, 43138, 43144, 43146, 43168, 43170, 43176, 43178, 43520, 43522,
    43528, 43530, 43552, 43554, 43560, 43562, 43648, 43650, 43656, 43658, 43680,
    43682, 43688, 43690};

}  // namespace encode

/// For decoding
namespace decode {

static const uint_fast8_t x[256] = {
    0,  1,  0,  1,  2,  3,  2,  3,  0,  1,  0,  1,  2,  3,  2,  3,  4,  5,  4,
    5,  6,  7,  6,  7,  4,  5,  4,  5,  6,  7,  6,  7,  0,  1,  0,  1,  2,  3,
    2,  3,  0,  1,  0,  1,  2,  3,  2,  3,  4,  5,  4,  5,  6,  7,  6,  7,  4,
    5,  4,  5,  6,  7,  6,  7,  8,  9,  8,  9,  10, 11, 10, 11, 8,  9,  8,  9,
    10, 11, 10, 11, 12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14,
    15, 8,  9,  8,  9,  10, 11, 10, 11, 8,  9,  8,  9,  10, 11, 10, 11, 12, 13,
    12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14, 15, 0,  1,  0,  1,  2,
    3,  2,  3,  0,  1,  0,  1,  2,  3,  2,  3,  4,  5,  4,  5,  6,  7,  6,  7,
    4,  5,  4,  5,  6,  7,  6,  7,  0,  1,  0,  1,  2,  3,  2,  3,  0,  1,  0,
    1,  2,  3,  2,  3,  4,  5,  4,  5,  6,  7,  6,  7,  4,  5,  4,  5,  6,  7,
    6,  7,  8,  9,  8,  9,  10, 11, 10, 11, 8,  9,  8,  9,  10, 11, 10, 11, 12,
    13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14, 15, 8,  9,  8,  9,
    10, 11, 10, 11, 8,  9,  8,  9,  10, 11, 10, 11, 12, 13, 12, 13, 14, 15, 14,
    15, 12, 13, 12, 13, 14, 15, 14, 15};

static const uint_fast8_t y[256] = {
    0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  3,  2,  2,  3,  3,  0,  0,  1,
    1,  0,  0,  1,  1,  2,  2,  3,  3,  2,  2,  3,  3,  4,  4,  5,  5,  4,  4,
    5,  5,  6,  6,  7,  7,  6,  6,  7,  7,  4,  4,  5,  5,  4,  4,  5,  5,  6,
    6,  7,  7,  6,  6,  7,  7,  0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  3,
    2,  2,  3,  3,  0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  3,  2,  2,  3,
    3,  4,  4,  5,  5,  4,  4,  5,  5,  6,  6,  7,  7,  6,  6,  7,  7,  4,  4,
    5,  5,  4,  4,  5,  5,  6,  6,  7,  7,  6,  6,  7,  7,  8,  8,  9,  9,  8,
    8,  9,  9,  10, 10, 11, 11, 10, 10, 11, 11, 8,  8,  9,  9,  8,  8,  9,  9,
    10, 10, 11, 11, 10, 10, 11, 11, 12, 12, 13, 13, 12, 12, 13, 13, 14, 14, 15,
    15, 14, 14, 15, 15, 12, 12, 13, 13, 12, 12, 13, 13, 14, 14, 15, 15, 14, 14,
    15, 15, 8,  8,  9,  9,  8,  8,  9,  9,  10, 10, 11, 11, 10, 10, 11, 11, 8,
    8,  9,  9,  8,  8,  9,  9,  10, 10, 11, 11, 10, 10, 11, 11, 12, 12, 13, 13,
    12, 12, 13, 13, 14, 14, 15, 15, 14, 14, 15, 15, 12, 12, 13, 13, 12, 12, 13,
    13, 14, 14, 15, 15, 14, 14, 15, 15};

}  // namespace decode

}  // namespace lookup_table

/// @brief Morton code implementation in two dimensions.
/// @tparam T Integral type for morton_code
/// @tparam U Integral type for coordinates
/// @tparam Tag Tag to switch implementations
template <typename T, typename U, typename Tag>
struct morton_impl {};

/// @brief Morton code implementation using pre-shifted look-up tables in two
/// dimensions.
/// @tparam T Integral type for morton_code
/// @tparam U Integral type for coordinates
template <typename T, typename U>
class morton_impl<T, U, tag::preshifted_lookup_table> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] c Coordinates
  /// @returns Moton code
  static morton_code<T> encode(const coordinates<U>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @returns Coordinates
  static coordinates<U> decode(const morton_code<T> m) noexcept;

 private:
  /// Helper function for decode
  /// @param[in] m Morton code
  /// @param[in] table Look-up table
  /// @returns Coordinate
  static U decode(const T m, const uint_fast8_t* table) noexcept;
};

template <typename T, typename U>
inline U morton_impl<T, U, tag::preshifted_lookup_table>::decode(
    const T m, const uint_fast8_t* table) noexcept {
  T code = 0;
  // 8-bit mask
  constexpr T mask = 0x000000FF;
  for (unsigned int i = 0; i < sizeof(U); ++i) {
    const unsigned int shift = i * 8;
    code |= static_cast<T>(table[(m >> shift) & mask] << (4 * i));
  }
  return static_cast<U>(code);
}

template <typename T, typename U>
inline morton_code<T> morton_impl<T, U, tag::preshifted_lookup_table>::encode(
    const coordinates<U>& c) noexcept {
  T code = 0;
  // 8-bit mask
  constexpr T mask = 0x000000FF;
  for (unsigned int i = sizeof(U); i > 0; --i) {
    const unsigned int shift = (i - 1) * 8;
    code = code << 16 |  //
           lookup_table::encode::y[(c.y >> shift) & mask] |
           lookup_table::encode::x[(c.x >> shift) & mask];
  }
  return morton_code<T>{code};
}

template <typename T, typename U>
inline coordinates<U> morton_impl<T, U, tag::preshifted_lookup_table>::decode(
    const morton_code<T> m) noexcept {
  return {decode(m.value, lookup_table::decode::x),
          decode(m.value, lookup_table::decode::y)};
}

/// @brief Morton code implementation using look-up tables in two dimensions.
/// @tparam T Morton code
/// @tparam Coordinate Coordinate
template <typename T, typename U>
class morton_impl<T, U, tag::lookup_table> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] Coordinates coordinates
  /// @returns Moton code
  static morton_code<T> encode(const coordinates<U>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @return Coordinates
  static coordinates<U> decode(const morton_code<T> m) noexcept;

 private:
  /// Helper function for decode
  /// @param[in] m Morton code
  /// @param[in] table Look-up table
  /// @param[in] shift0 Start shift
  /// @returns Coordinate
  static U decode(const T m, const uint_fast8_t* table,
                  const unsigned int shift0) noexcept;
};

template <typename T, typename U>
inline morton_code<T> morton_impl<T, U, tag::lookup_table>::encode(
    const coordinates<U>& c) noexcept {
  T code = 0;
  // 8-bit mask
  constexpr T mask = 0x000000FF;
  for (unsigned int i = sizeof(U); i > 0; --i) {
    const unsigned int shift = (i - 1) * 8;
    code = code << 16 |  //
           (lookup_table::encode::x[(c.y >> shift) & mask] << T(1)) |
           lookup_table::encode::x[(c.x >> shift) & mask];
  }
  return morton_code<T>{code};
}

template <typename T, typename U>
inline U morton_impl<T, U, tag::lookup_table>::decode(
    const T m, const uint_fast8_t* table, const unsigned int shift0) noexcept {
  T code = 0;
  // 8-bit mask
  constexpr T mask = 0x000000FF;
  for (unsigned int i = 0; i < sizeof(T); ++i) {
    const unsigned int shift = i * 8 + shift0;
    code |= static_cast<T>(table[(m >> shift) & mask] << (4 * i));
  }
  return static_cast<U>(code);
}

template <typename T, typename U>
inline coordinates<U> morton_impl<T, U, tag::lookup_table>::decode(  //
    const morton_code<T> m) noexcept {
  return {decode(m.value, lookup_table::decode::x, 0),
          decode(m.value, lookup_table::decode::x, 1)};
}

#ifdef MORTON2D_USE_BMI

inline uint32_t pdep(uint32_t source, uint32_t mask) noexcept {
  return _pdep_u32(source, mask);
}

inline uint64_t pdep(uint64_t source, uint64_t mask) noexcept {
  return _pdep_u64(source, mask);
}

inline uint32_t pext(uint32_t source, uint32_t mask) noexcept {
  return _pext_u32(source, mask);
}

inline uint64_t pext(uint64_t source, uint64_t mask) noexcept {
  return _pext_u64(source, mask);
}

/// @brief Morton code implementation using BMI instruction sets.
/// @tparam T Integral type for morton_code
/// @tparam U Integral type for coordinates
template <typename T, typename U>
class morton_impl<T, U, tag::bmi> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] c Coordinates
  /// @returns Moton code
  static morton_code<T> encode(const coordinates<U>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @returns Coordinates
  static coordinates<U> decode(const morton_code<T> m) noexcept;
};

template <typename T, typename U>
inline morton_code<T> morton_impl<T, U, tag::bmi>::encode(
    const coordinates<U>& c) noexcept {
  T m = 0;
  constexpr T mask_x = 0x5555555555555555;
  constexpr T mask_y = 0xAAAAAAAAAAAAAAAA;
  m |= pdep(static_cast<T>(c.x), mask_x) | pdep(static_cast<T>(c.y), mask_y);
  return morton_code<T>{m};
}

template <typename T, typename U>
inline coordinates<U> morton_impl<T, U, tag::bmi>::decode(
    const morton_code<T> m) noexcept {
  constexpr T mask_x = 0x5555555555555555;
  constexpr T mask_y = 0xAAAAAAAAAAAAAAAA;
  return {static_cast<U>(pext(m.value, mask_x)),
          static_cast<U>(pext(m.value, mask_y))};
}
#endif  // MORTON2D_USE_BMI

}  // namespace detail

/// @brief Encode 2D coordinates into 32-bits morton code.
/// @tparam Tag Tag to switch implementation
/// @param[in] Coordinates Coordinates
/// @returns Morton code
template <typename Tag>
inline morton_code32_t encode(const coordinates16_t& c, Tag) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  return detail::morton_impl<uint_fast32_t, uint_fast16_t, Tag>::encode(c);
}

/// @brief Encode 2D coordinates into 66-bits morton code.
/// @tparam Tag Tag to switch implementation
/// @param[in] Coordinates Coordinates
/// @returns Morton code
template <typename Tag>
inline morton_code64_t encode(const coordinates32_t& c, Tag) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  return detail::morton_impl<uint_fast64_t, uint_fast32_t, Tag>::encode(c);
}

/// @brief Encode 2D coordinates into 32-bits morton code.
/// @param[in] Coordinates Coordinates
/// @returns Morton code
inline morton_code32_t encode(const coordinates16_t& c) noexcept {
#ifdef MORTON2D_USE_BMI
  return encode(c, tag::bmi{});
#else
  return encode(c, tag::preshifted_lookup_table{});
#endif  // MORTON2D_USE_BMI
}

/// @brief Encode 2D coordinates into 64-bits morton code.
/// @param[in] Coordinates Coordinates
/// @returns Morton code
inline morton_code64_t encode(const coordinates32_t& c) noexcept {
#ifdef MORTON2D_USE_BMI
  return encode(c, tag::bmi{});
#else
  return encode(c, tag::preshifted_lookup_table{});
#endif  // MORTON2D_USE_BMI
}

/// @brief Decode 32-bits morton code into 2D coordinates.
/// @tparam Tag Tag to switch implementations
/// @param[in] m Morton code
/// @returns Coordinates
template <typename Tag>
inline coordinates16_t decode(const morton_code32_t m, Tag) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  return detail::morton_impl<uint_fast32_t, uint_fast16_t, Tag>::decode(m);
}

/// @brief Decode 64-bits morton code into 2D coordinates.
/// @tparam Tag Tag to switch implementations
/// @param[in] m Morton code
/// @returns Coordinates
template <typename Tag>
inline coordinates32_t decode(const morton_code64_t m, Tag) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  return detail::morton_impl<uint_fast64_t, uint_fast32_t, Tag>::decode(m);
}

/// @brief Decode 32-bits morton code into 2D coordinates.
/// @param[in] m Morton code
/// @returns Coordinates
inline coordinates16_t decode(const morton_code32_t m) noexcept {
#ifdef MORTON2D_USE_BMI
  return decode(m, tag::bmi{});
#else
  return decode(m, tag::preshifted_lookup_table{});
#endif  // MORTON2D_USE_BMI
}

/// @brief Decode 64-bits morton code into 2D coordinates.
/// @param[in] m Morton code
/// @returns Coordinates
inline coordinates32_t decode(const morton_code64_t m) noexcept {
#ifdef MORTON2D_USE_BMI
  return decode(m, tag::bmi{});
#else
  return decode(m, tag::preshifted_lookup_table{});
#endif  // MORTON2D_USE_BMI
}

}  // namespace morton2d

#endif  // MORTON_MORTON2D_HPP