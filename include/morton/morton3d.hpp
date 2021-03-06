// This software is released under the MIT license.
//
// Copyright (c) 2016 Jeroen Baert
// Copyright (c) 2020 Sho Hirose
#ifndef MORTON_MORTON3D_HPP
#define MORTON_MORTON3D_HPP

#if defined(__BMI2__) || __AVX2__
#define MORTON3D_USE_BMI
#include <immintrin.h>
#endif

#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <type_traits>

/// Moton 3D namespace
namespace morton3d {

/// Tags for partial specialization of morton2d and morton3d classes.
namespace tag {

/// Tag for the implementation using pre-shifted look-up table
struct preshifted_lookup_table {};

/// Tag for the implementation using look-up table
struct lookup_table {};

/// Tag for the implementation using BMI instructions
struct bmi {};

/// Tag for matic bits implementation
struct magic_bits {};

}  // namespace tag

/// Default tag
#ifdef MORTON3D_USE_BMI
using default_tag = tag::bmi;
#else
using default_tag = tag::preshifted_lookup_table;
#endif  // MORTON3D_USE_BMI

/// @brief Check if a given type is a tag.
/// @tparam Tag Tag type
template <typename Tag>
struct is_tag : std::conditional<
                    (std::is_same<Tag, tag::bmi>::value ||
                     std::is_same<Tag, tag::preshifted_lookup_table>::value ||
                     std::is_same<Tag, tag::lookup_table>::value ||
                     std::is_same<Tag, tag::magic_bits>::value),
                    std::true_type, std::false_type>::type {};

/// @brief Morton code
/// @tparam T Value type
template <typename T>
struct morton_code {
  static_assert(std::is_integral<T>::value, "T is not an integral type");
  using value_type = T;

  T value;

  /// @param[in] tvalue Value
  explicit morton_code(T tvalue) noexcept : value{tvalue} {}

  morton_code() = default;
  morton_code(const morton_code&) = default;
  morton_code(morton_code&&) = default;

  morton_code& operator=(const morton_code&) = default;
  morton_code& operator=(morton_code&&) = default;

  /// @brief Explicit conversion operator
  explicit operator T() const noexcept { return value; }
};

/// Morton code in 32 bits
using morton_code32_t = morton_code<uint32_t>;
/// Morton code in 64 bits
using morton_code64_t = morton_code<uint64_t>;

template <typename T>
bool operator==(const morton_code<T> m1, const morton_code<T> m2) noexcept {
  return m1.value == m2.value;
}

template <typename T>
bool operator!=(const morton_code<T> m1, const morton_code<T> m2) noexcept {
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

/// @brief Coordinates in three dimensions
/// @tparam T Value type
template <typename T>
struct coordinates {
  static_assert(std::is_integral<T>::value, "T is not an integral type");
  using value_type = T;

  T x;  /// X coordinate
  T y;  /// Y coordinate
  T z;  /// Z coordinate

  /// @param[in] tx X coordinate
  /// @param[in] ty Y coordinate
  /// @param[in] tz Z coordinate
  coordinates(T tx, T ty, T tz) noexcept : x{tx}, y{ty}, z{tz} {}

  coordinates() = default;
  coordinates(const coordinates&) = default;
  coordinates(coordinates&&) = default;

  coordinates& operator=(const coordinates&) = default;
  coordinates& operator=(coordinates&&) = default;
};

/// Coordinates in 16 bits
using coordinates16_t = coordinates<uint16_t>;
/// Coordinates in 32 bits
using coordinates32_t = coordinates<uint32_t>;

template <typename T>
bool operator==(const coordinates<T>& c1, const coordinates<T>& c2) noexcept {
  return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z;
}

template <typename T>
bool operator!=(const coordinates<T>& c1, const coordinates<T>& c2) noexcept {
  return !(c1 == c2);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const coordinates<T>& c) {
  return os << "(" << c.x << ", " << c.y << ", " << c.z << ")";
}

template <typename T>
std::istream& operator>>(std::istream& is, coordinates<T>& c) {
  return is >> c.x >> c.y >> c.z;
}

/// Detail implementation of morton library
namespace detail {

/// Look-up tables
namespace lookup_table {

/// For endocding
namespace encode {

static const uint32_t x[256] = {
    0x00000000, 0x00000001, 0x00000008, 0x00000009, 0x00000040, 0x00000041,
    0x00000048, 0x00000049, 0x00000200, 0x00000201, 0x00000208, 0x00000209,
    0x00000240, 0x00000241, 0x00000248, 0x00000249, 0x00001000, 0x00001001,
    0x00001008, 0x00001009, 0x00001040, 0x00001041, 0x00001048, 0x00001049,
    0x00001200, 0x00001201, 0x00001208, 0x00001209, 0x00001240, 0x00001241,
    0x00001248, 0x00001249, 0x00008000, 0x00008001, 0x00008008, 0x00008009,
    0x00008040, 0x00008041, 0x00008048, 0x00008049, 0x00008200, 0x00008201,
    0x00008208, 0x00008209, 0x00008240, 0x00008241, 0x00008248, 0x00008249,
    0x00009000, 0x00009001, 0x00009008, 0x00009009, 0x00009040, 0x00009041,
    0x00009048, 0x00009049, 0x00009200, 0x00009201, 0x00009208, 0x00009209,
    0x00009240, 0x00009241, 0x00009248, 0x00009249, 0x00040000, 0x00040001,
    0x00040008, 0x00040009, 0x00040040, 0x00040041, 0x00040048, 0x00040049,
    0x00040200, 0x00040201, 0x00040208, 0x00040209, 0x00040240, 0x00040241,
    0x00040248, 0x00040249, 0x00041000, 0x00041001, 0x00041008, 0x00041009,
    0x00041040, 0x00041041, 0x00041048, 0x00041049, 0x00041200, 0x00041201,
    0x00041208, 0x00041209, 0x00041240, 0x00041241, 0x00041248, 0x00041249,
    0x00048000, 0x00048001, 0x00048008, 0x00048009, 0x00048040, 0x00048041,
    0x00048048, 0x00048049, 0x00048200, 0x00048201, 0x00048208, 0x00048209,
    0x00048240, 0x00048241, 0x00048248, 0x00048249, 0x00049000, 0x00049001,
    0x00049008, 0x00049009, 0x00049040, 0x00049041, 0x00049048, 0x00049049,
    0x00049200, 0x00049201, 0x00049208, 0x00049209, 0x00049240, 0x00049241,
    0x00049248, 0x00049249, 0x00200000, 0x00200001, 0x00200008, 0x00200009,
    0x00200040, 0x00200041, 0x00200048, 0x00200049, 0x00200200, 0x00200201,
    0x00200208, 0x00200209, 0x00200240, 0x00200241, 0x00200248, 0x00200249,
    0x00201000, 0x00201001, 0x00201008, 0x00201009, 0x00201040, 0x00201041,
    0x00201048, 0x00201049, 0x00201200, 0x00201201, 0x00201208, 0x00201209,
    0x00201240, 0x00201241, 0x00201248, 0x00201249, 0x00208000, 0x00208001,
    0x00208008, 0x00208009, 0x00208040, 0x00208041, 0x00208048, 0x00208049,
    0x00208200, 0x00208201, 0x00208208, 0x00208209, 0x00208240, 0x00208241,
    0x00208248, 0x00208249, 0x00209000, 0x00209001, 0x00209008, 0x00209009,
    0x00209040, 0x00209041, 0x00209048, 0x00209049, 0x00209200, 0x00209201,
    0x00209208, 0x00209209, 0x00209240, 0x00209241, 0x00209248, 0x00209249,
    0x00240000, 0x00240001, 0x00240008, 0x00240009, 0x00240040, 0x00240041,
    0x00240048, 0x00240049, 0x00240200, 0x00240201, 0x00240208, 0x00240209,
    0x00240240, 0x00240241, 0x00240248, 0x00240249, 0x00241000, 0x00241001,
    0x00241008, 0x00241009, 0x00241040, 0x00241041, 0x00241048, 0x00241049,
    0x00241200, 0x00241201, 0x00241208, 0x00241209, 0x00241240, 0x00241241,
    0x00241248, 0x00241249, 0x00248000, 0x00248001, 0x00248008, 0x00248009,
    0x00248040, 0x00248041, 0x00248048, 0x00248049, 0x00248200, 0x00248201,
    0x00248208, 0x00248209, 0x00248240, 0x00248241, 0x00248248, 0x00248249,
    0x00249000, 0x00249001, 0x00249008, 0x00249009, 0x00249040, 0x00249041,
    0x00249048, 0x00249049, 0x00249200, 0x00249201, 0x00249208, 0x00249209,
    0x00249240, 0x00249241, 0x00249248, 0x00249249};

static const uint32_t y[256] = {
    0x00000000, 0x00000002, 0x00000010, 0x00000012, 0x00000080, 0x00000082,
    0x00000090, 0x00000092, 0x00000400, 0x00000402, 0x00000410, 0x00000412,
    0x00000480, 0x00000482, 0x00000490, 0x00000492, 0x00002000, 0x00002002,
    0x00002010, 0x00002012, 0x00002080, 0x00002082, 0x00002090, 0x00002092,
    0x00002400, 0x00002402, 0x00002410, 0x00002412, 0x00002480, 0x00002482,
    0x00002490, 0x00002492, 0x00010000, 0x00010002, 0x00010010, 0x00010012,
    0x00010080, 0x00010082, 0x00010090, 0x00010092, 0x00010400, 0x00010402,
    0x00010410, 0x00010412, 0x00010480, 0x00010482, 0x00010490, 0x00010492,
    0x00012000, 0x00012002, 0x00012010, 0x00012012, 0x00012080, 0x00012082,
    0x00012090, 0x00012092, 0x00012400, 0x00012402, 0x00012410, 0x00012412,
    0x00012480, 0x00012482, 0x00012490, 0x00012492, 0x00080000, 0x00080002,
    0x00080010, 0x00080012, 0x00080080, 0x00080082, 0x00080090, 0x00080092,
    0x00080400, 0x00080402, 0x00080410, 0x00080412, 0x00080480, 0x00080482,
    0x00080490, 0x00080492, 0x00082000, 0x00082002, 0x00082010, 0x00082012,
    0x00082080, 0x00082082, 0x00082090, 0x00082092, 0x00082400, 0x00082402,
    0x00082410, 0x00082412, 0x00082480, 0x00082482, 0x00082490, 0x00082492,
    0x00090000, 0x00090002, 0x00090010, 0x00090012, 0x00090080, 0x00090082,
    0x00090090, 0x00090092, 0x00090400, 0x00090402, 0x00090410, 0x00090412,
    0x00090480, 0x00090482, 0x00090490, 0x00090492, 0x00092000, 0x00092002,
    0x00092010, 0x00092012, 0x00092080, 0x00092082, 0x00092090, 0x00092092,
    0x00092400, 0x00092402, 0x00092410, 0x00092412, 0x00092480, 0x00092482,
    0x00092490, 0x00092492, 0x00400000, 0x00400002, 0x00400010, 0x00400012,
    0x00400080, 0x00400082, 0x00400090, 0x00400092, 0x00400400, 0x00400402,
    0x00400410, 0x00400412, 0x00400480, 0x00400482, 0x00400490, 0x00400492,
    0x00402000, 0x00402002, 0x00402010, 0x00402012, 0x00402080, 0x00402082,
    0x00402090, 0x00402092, 0x00402400, 0x00402402, 0x00402410, 0x00402412,
    0x00402480, 0x00402482, 0x00402490, 0x00402492, 0x00410000, 0x00410002,
    0x00410010, 0x00410012, 0x00410080, 0x00410082, 0x00410090, 0x00410092,
    0x00410400, 0x00410402, 0x00410410, 0x00410412, 0x00410480, 0x00410482,
    0x00410490, 0x00410492, 0x00412000, 0x00412002, 0x00412010, 0x00412012,
    0x00412080, 0x00412082, 0x00412090, 0x00412092, 0x00412400, 0x00412402,
    0x00412410, 0x00412412, 0x00412480, 0x00412482, 0x00412490, 0x00412492,
    0x00480000, 0x00480002, 0x00480010, 0x00480012, 0x00480080, 0x00480082,
    0x00480090, 0x00480092, 0x00480400, 0x00480402, 0x00480410, 0x00480412,
    0x00480480, 0x00480482, 0x00480490, 0x00480492, 0x00482000, 0x00482002,
    0x00482010, 0x00482012, 0x00482080, 0x00482082, 0x00482090, 0x00482092,
    0x00482400, 0x00482402, 0x00482410, 0x00482412, 0x00482480, 0x00482482,
    0x00482490, 0x00482492, 0x00490000, 0x00490002, 0x00490010, 0x00490012,
    0x00490080, 0x00490082, 0x00490090, 0x00490092, 0x00490400, 0x00490402,
    0x00490410, 0x00490412, 0x00490480, 0x00490482, 0x00490490, 0x00490492,
    0x00492000, 0x00492002, 0x00492010, 0x00492012, 0x00492080, 0x00492082,
    0x00492090, 0x00492092, 0x00492400, 0x00492402, 0x00492410, 0x00492412,
    0x00492480, 0x00492482, 0x00492490, 0x00492492};

static const uint32_t z[256] = {
    0x00000000, 0x00000004, 0x00000020, 0x00000024, 0x00000100, 0x00000104,
    0x00000120, 0x00000124, 0x00000800, 0x00000804, 0x00000820, 0x00000824,
    0x00000900, 0x00000904, 0x00000920, 0x00000924, 0x00004000, 0x00004004,
    0x00004020, 0x00004024, 0x00004100, 0x00004104, 0x00004120, 0x00004124,
    0x00004800, 0x00004804, 0x00004820, 0x00004824, 0x00004900, 0x00004904,
    0x00004920, 0x00004924, 0x00020000, 0x00020004, 0x00020020, 0x00020024,
    0x00020100, 0x00020104, 0x00020120, 0x00020124, 0x00020800, 0x00020804,
    0x00020820, 0x00020824, 0x00020900, 0x00020904, 0x00020920, 0x00020924,
    0x00024000, 0x00024004, 0x00024020, 0x00024024, 0x00024100, 0x00024104,
    0x00024120, 0x00024124, 0x00024800, 0x00024804, 0x00024820, 0x00024824,
    0x00024900, 0x00024904, 0x00024920, 0x00024924, 0x00100000, 0x00100004,
    0x00100020, 0x00100024, 0x00100100, 0x00100104, 0x00100120, 0x00100124,
    0x00100800, 0x00100804, 0x00100820, 0x00100824, 0x00100900, 0x00100904,
    0x00100920, 0x00100924, 0x00104000, 0x00104004, 0x00104020, 0x00104024,
    0x00104100, 0x00104104, 0x00104120, 0x00104124, 0x00104800, 0x00104804,
    0x00104820, 0x00104824, 0x00104900, 0x00104904, 0x00104920, 0x00104924,
    0x00120000, 0x00120004, 0x00120020, 0x00120024, 0x00120100, 0x00120104,
    0x00120120, 0x00120124, 0x00120800, 0x00120804, 0x00120820, 0x00120824,
    0x00120900, 0x00120904, 0x00120920, 0x00120924, 0x00124000, 0x00124004,
    0x00124020, 0x00124024, 0x00124100, 0x00124104, 0x00124120, 0x00124124,
    0x00124800, 0x00124804, 0x00124820, 0x00124824, 0x00124900, 0x00124904,
    0x00124920, 0x00124924, 0x00800000, 0x00800004, 0x00800020, 0x00800024,
    0x00800100, 0x00800104, 0x00800120, 0x00800124, 0x00800800, 0x00800804,
    0x00800820, 0x00800824, 0x00800900, 0x00800904, 0x00800920, 0x00800924,
    0x00804000, 0x00804004, 0x00804020, 0x00804024, 0x00804100, 0x00804104,
    0x00804120, 0x00804124, 0x00804800, 0x00804804, 0x00804820, 0x00804824,
    0x00804900, 0x00804904, 0x00804920, 0x00804924, 0x00820000, 0x00820004,
    0x00820020, 0x00820024, 0x00820100, 0x00820104, 0x00820120, 0x00820124,
    0x00820800, 0x00820804, 0x00820820, 0x00820824, 0x00820900, 0x00820904,
    0x00820920, 0x00820924, 0x00824000, 0x00824004, 0x00824020, 0x00824024,
    0x00824100, 0x00824104, 0x00824120, 0x00824124, 0x00824800, 0x00824804,
    0x00824820, 0x00824824, 0x00824900, 0x00824904, 0x00824920, 0x00824924,
    0x00900000, 0x00900004, 0x00900020, 0x00900024, 0x00900100, 0x00900104,
    0x00900120, 0x00900124, 0x00900800, 0x00900804, 0x00900820, 0x00900824,
    0x00900900, 0x00900904, 0x00900920, 0x00900924, 0x00904000, 0x00904004,
    0x00904020, 0x00904024, 0x00904100, 0x00904104, 0x00904120, 0x00904124,
    0x00904800, 0x00904804, 0x00904820, 0x00904824, 0x00904900, 0x00904904,
    0x00904920, 0x00904924, 0x00920000, 0x00920004, 0x00920020, 0x00920024,
    0x00920100, 0x00920104, 0x00920120, 0x00920124, 0x00920800, 0x00920804,
    0x00920820, 0x00920824, 0x00920900, 0x00920904, 0x00920920, 0x00920924,
    0x00924000, 0x00924004, 0x00924020, 0x00924024, 0x00924100, 0x00924104,
    0x00924120, 0x00924124, 0x00924800, 0x00924804, 0x00924820, 0x00924824,
    0x00924900, 0x00924904, 0x00924920, 0x00924924};

}  // namespace encode

/// For decoding
namespace decode {

static const uint8_t x[512] = {
    0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2,
    3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1,
    0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6,
    7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5,
    4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6,
    7, 6, 7, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1,
    0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2,
    3, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5,
    6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4,
    5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7,
    6, 7, 6, 7, 6, 7, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0,
    1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3,
    2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4,
    5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7,
    6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4,
    5, 6, 7, 6, 7, 6, 7, 6, 7, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3,
    0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2,
    3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5,
    4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6,
    7, 6, 7, 6, 7, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 4, 5, 4, 5,
    4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7};

static const uint8_t y[512] = {
    0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 2,
    2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2,
    3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1,
    1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1,
    0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2,
    2, 3, 3, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6,
    7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5,
    5, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 4, 4, 5, 5, 4, 4, 5, 5,
    4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 4,
    4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6,
    7, 7, 6, 6, 7, 7, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3,
    3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
    0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1, 0,
    0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2,
    3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3,
    3, 2, 2, 3, 3, 2, 2, 3, 3, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5,
    6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 4, 4, 5, 5, 4, 4, 5, 5, 4,
    4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7, 4, 4,
    5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7,
    7, 6, 6, 7, 7, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7,
    6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 7, 7};

static const uint8_t z[512] = {
    0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2,
    2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3,
    3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3,
    3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 2,
    2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2,
    2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4,
    4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6,
    7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 4, 4, 4, 4, 5,
    5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5,
    5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7,
    7, 6, 6, 6, 6, 7, 7, 7, 7, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5,
    4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 6,
    6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 4, 4,
    4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4,
    4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6,
    7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7};

}  // namespace decode

}  // namespace lookup_table

/// @brief Morton code implementation in three dimensions.
/// @tparam T Integral type for morton_code
/// @tparam U Integral type for coordinates
/// @tparam Tag Tag to switch implementations
template <typename T, typename U, typename Tag>
class morton3d {};

/// @brief Morton code implementation using pre-shifted look-up tables in three
/// dimensions.
/// @tparam T Integral type for morton_code
/// @tparam U Integral type for coordinates
template <typename T, typename U>
class morton3d<T, U, tag::preshifted_lookup_table> {
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
  static U decode(const T m, const uint8_t* table) noexcept;
};

template <typename T, typename U>
inline morton_code<T> morton3d<T, U, tag::preshifted_lookup_table>::encode(
    const coordinates<U>& c) noexcept {
  T code = 0;
  // 8-bit mask
  constexpr T mask = 0x000000FF;
  for (unsigned int i = sizeof(U); i > 0; --i) {
    unsigned int shift = (i - 1) * 8;
    code = code << 24 | (lookup_table::encode::z[(c.z >> shift) & mask] |
                         lookup_table::encode::y[(c.y >> shift) & mask] |
                         lookup_table::encode::x[(c.x >> shift) & mask]);
  }
  return morton_code<T>{code};
}

template <typename T, typename U>
inline U morton3d<T, U, tag::preshifted_lookup_table>::decode(
    const T m, const uint8_t* table) noexcept {
  T code = 0;
  // ceil for 32bit, floor for 64bit
  const unsigned int loops = (sizeof(T) <= 4) ? 4 : 7;
  // 9-bit mask
  constexpr T mask = 0x000001FF;
  for (unsigned int i = 0; i < loops; ++i) {
    const unsigned int shift = i * 9;
    code |= static_cast<T>(table[(m >> shift) & mask] << T(3 * i));
  }
  return static_cast<U>(code);
}

template <typename T, typename U>
inline coordinates<U> morton3d<T, U, tag::preshifted_lookup_table>::decode(
    const morton_code<T> m) noexcept {
  return {decode(m.value, lookup_table::decode::x),
          decode(m.value, lookup_table::decode::y),
          decode(m.value, lookup_table::decode::z)};
}

/// @brief Morton code implementation using look-up tables in three dimensions.
/// @tparam T Integral type for morton_code
/// @tparam U Integral type for coordinates
template <typename T, typename U>
class morton3d<T, U, tag::lookup_table> {
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
  /// @param[in] shift0 Start shift
  /// @returns Coordinate
  static U decode(const T m, const uint8_t* table,
                  const unsigned int shift0) noexcept;
};

template <typename T, typename U>
inline morton_code<T> morton3d<T, U, tag::lookup_table>::encode(
    const coordinates<U>& c) noexcept {
  T code = 0;
  // 8-bit mask
  constexpr T mask = 0x000000FF;
  for (unsigned int i = sizeof(U); i > 0; --i) {
    unsigned int shift = (i - 1) * 8;
    code =
        code << 24 | ((lookup_table::encode::x[(c.z >> shift) & mask] << T(2)) |
                      (lookup_table::encode::x[(c.y >> shift) & mask] << T(1)) |
                      lookup_table::encode::x[(c.x >> shift) & mask]);
  }
  return morton_code<T>{code};
}

template <typename T, typename U>
inline U morton3d<T, U, tag::lookup_table>::decode(
    const T m, const uint8_t* table, const unsigned int shift0) noexcept {
  T code = 0;
  // ceil for 32bit, floor for 64bit
  const unsigned int loops = (sizeof(T) <= 4) ? 4 : 7;
  // 9-bit mask
  constexpr T mask = 0x000001FF;
  for (unsigned int i = 0; i < loops; ++i) {
    const unsigned int shift = i * 9 + shift0;
    code |= static_cast<T>(table[(m >> shift) & mask] << T(3 * i));
  }
  return static_cast<U>(code);
}

template <typename T, typename U>
inline coordinates<U> morton3d<T, U, tag::lookup_table>::decode(
    const morton_code<T> m) noexcept {
  return {decode(m.value, lookup_table::decode::x, 0),
          decode(m.value, lookup_table::decode::x, 1),
          decode(m.value, lookup_table::decode::x, 2)};
}

#ifdef MORTON3D_USE_BMI

/// @brief Morton code implementation using BMI instruction sets for 32-bit
/// morton codes.
template <>
class morton3d<uint32_t, uint16_t, tag::bmi> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] c Coordinates
  /// @returns Moton code
  static morton_code<uint32_t> encode(const coordinates<uint16_t>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @returns Coordinates
  static coordinates<uint16_t> decode(const morton_code<uint32_t> m) noexcept;

 private:
  static constexpr uint32_t mask_x = 0x49249249;
  static constexpr uint32_t mask_y = 0x92492492;
  static constexpr uint32_t mask_z = 0x24924924;
};

inline morton_code<uint32_t> morton3d<uint32_t, uint16_t, tag::bmi>::encode(
    const coordinates<uint16_t>& c) noexcept {
  uint32_t m = 0;
  m |= _pdep_u32(static_cast<uint32_t>(c.x), mask_x) |
       _pdep_u32(static_cast<uint32_t>(c.y), mask_y) |
       _pdep_u32(static_cast<uint32_t>(c.z), mask_z);
  return morton_code<uint32_t>{m};
}

inline coordinates<uint16_t> morton3d<uint32_t, uint16_t, tag::bmi>::decode(
    const morton_code<uint32_t> m) noexcept {
  return {static_cast<uint16_t>(_pext_u32(m.value, mask_x)),
          static_cast<uint16_t>(_pext_u32(m.value, mask_y)),
          static_cast<uint16_t>(_pext_u32(m.value, mask_z))};
}

/// @brief Morton code implementation using BMI instruction sets for 64-bit
/// morton codes.
template <>
class morton3d<uint64_t, uint32_t, tag::bmi> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] c Coordinates
  /// @returns Moton code
  static morton_code<uint64_t> encode(const coordinates<uint32_t>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @returns Coordinates
  static coordinates<uint32_t> decode(const morton_code<uint64_t> m) noexcept;

 private:
  static constexpr uint64_t mask_x = 0x9249249249249249;
  static constexpr uint64_t mask_y = 0x2492492492492492;
  static constexpr uint64_t mask_z = 0x4924924924924924;
};

inline morton_code<uint64_t> morton3d<uint64_t, uint32_t, tag::bmi>::encode(
    const coordinates<uint32_t>& c) noexcept {
  uint64_t m = 0;
  m |= _pdep_u64(static_cast<uint64_t>(c.x), mask_x) |
       _pdep_u64(static_cast<uint64_t>(c.y), mask_y) |
       _pdep_u64(static_cast<uint64_t>(c.z), mask_z);
  return morton_code<uint64_t>{m};
}

inline coordinates<uint32_t> morton3d<uint64_t, uint32_t, tag::bmi>::decode(
    const morton_code<uint64_t> m) noexcept {
  return {static_cast<uint32_t>(_pext_u64(m.value, mask_x)),
          static_cast<uint32_t>(_pext_u64(m.value, mask_y)),
          static_cast<uint32_t>(_pext_u64(m.value, mask_z))};
}

#endif  // MORTON3D_USE_BMI

/// @brief Morton code implementation using magic bits for 32 bits morton code.
template <>
class morton3d<uint32_t, uint16_t, tag::magic_bits> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] c Coordinates
  /// @returns Moton code
  static morton_code<uint32_t> encode(const coordinates<uint16_t>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @returns Coordinates
  static coordinates<uint16_t> decode(const morton_code<uint32_t> m) noexcept;

 private:
  /// @brief Split into every third bit.
  /// @param[in] c Coordinate
  /// @returns Morton code
  static uint32_t split_into_every_third_bit(const uint16_t c) noexcept;

  /// @brief Collect every third bit.
  /// @param[in] m Morton code
  /// @returns Coordinate
  static uint16_t collect_every_third_bit(const uint32_t m) noexcept;
};

inline morton_code<uint32_t>
morton3d<uint32_t, uint16_t, tag::magic_bits>::encode(
    const coordinates<uint16_t>& c) noexcept {
  return morton_code<uint32_t>{split_into_every_third_bit(c.x) |
                               (split_into_every_third_bit(c.y) << 1) |
                               (split_into_every_third_bit(c.z) << 2)};
}

inline coordinates<uint16_t>
morton3d<uint32_t, uint16_t, tag::magic_bits>::decode(
    const morton_code<uint32_t> m) noexcept {
  return {collect_every_third_bit(m.value),
          collect_every_third_bit(m.value >> 1),
          collect_every_third_bit(m.value >> 2)};
}

inline uint32_t
morton3d<uint32_t, uint16_t, tag::magic_bits>::split_into_every_third_bit(
    const uint16_t c) noexcept {
  uint32_t x = c;
  x &= 0x00000fff;
  x = (x | x << 16) & 0xff0000ff;
  x = (x | x << 8) & 0x0f00f00f;
  x = (x | x << 4) & 0xc30c30c3;
  x = (x | x << 2) & 0x49249249;
  return x;
}

inline uint16_t
morton3d<uint32_t, uint16_t, tag::magic_bits>::collect_every_third_bit(
    const uint32_t m) noexcept {
  uint32_t x = m;
  x &= 0x49249249;
  x = (x ^ (x >> 2)) & 0xc30c30c3;
  x = (x ^ (x >> 4)) & 0x0f00f00f;
  x = (x ^ (x >> 8)) & 0xff0000ff;
  x = (x ^ (x >> 16)) & 0x00000fff;
  return static_cast<uint16_t>(x);
}

/// @brief Morton code implementation using magic bits for 64 bits morton code.
template <>
class morton3d<uint64_t, uint32_t, tag::magic_bits> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] c Coordinates
  /// @returns Moton code
  static morton_code<uint64_t> encode(const coordinates<uint32_t>& c) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @returns Coordinates
  static coordinates<uint32_t> decode(const morton_code<uint64_t> m) noexcept;

 private:
  /// @brief Split into every third bit.
  /// @param[in] c Coordinate
  /// @returns Morton code
  static uint64_t split_into_every_third_bit(const uint32_t c) noexcept;

  /// @brief Collect every third bit.
  /// @param[in] m Morton code
  /// @returns Coordinate
  static uint32_t collect_every_third_bit(const uint64_t m) noexcept;
};

inline morton_code<uint64_t>
morton3d<uint64_t, uint32_t, tag::magic_bits>::encode(
    const coordinates<uint32_t>& c) noexcept {
  return morton_code<uint64_t>{split_into_every_third_bit(c.x) |
                               (split_into_every_third_bit(c.y) << 1) |
                               (split_into_every_third_bit(c.z) << 2)};
}

inline coordinates<uint32_t>
morton3d<uint64_t, uint32_t, tag::magic_bits>::decode(
    const morton_code<uint64_t> m) noexcept {
  return {collect_every_third_bit(m.value),
          collect_every_third_bit(m.value >> 1),
          collect_every_third_bit(m.value >> 2)};
}

inline uint64_t
morton3d<uint64_t, uint32_t, tag::magic_bits>::split_into_every_third_bit(
    const uint32_t c) noexcept {
  uint64_t x = c;
  x &= 0x00000000001fffff;
  x = (x | x << 32) & 0x001f00000000ffff;
  x = (x | x << 16) & 0x001f0000ff0000ff;
  x = (x | x << 8) & 0x100f00f00f00f00f;
  x = (x | x << 4) & 0x10c30c30c30c30c3;
  x = (x | x << 2) & 0x1249249249249249;
  return x;
}

inline uint32_t
morton3d<uint64_t, uint32_t, tag::magic_bits>::collect_every_third_bit(
    const uint64_t m) noexcept {
  uint64_t x = m;
  x &= 0x1249249249249249;
  x = (x ^ (x >> 2)) & 0x10c30c30c30c30c3;
  x = (x ^ (x >> 4)) & 0x100f00f00f00f00f;
  x = (x ^ (x >> 8)) & 0x001f0000ff0000ff;
  x = (x ^ (x >> 16)) & 0x001f00000000ffff;
  x = (x ^ (x >> 32)) & 0x00000000001fffff;
  return static_cast<uint32_t>(x);
}

}  // namespace detail

/// @brief Encode 3D coordinates into 32-bits morton code
/// @tparam Tag Tag to switch implementations
/// @param[in] c Coordinates
/// @returns Morton code
template <typename Tag = default_tag>
inline morton_code32_t encode(const coordinates16_t& c, Tag = Tag{}) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  assert(c.x < (1U << 10) &&
         "Maximum x coordinate is 2^10 - 1 for 32 bits encoding");
  assert(c.y < (1U << 10) &&
         "Maximum y coordinate is 2^10 - 1 for 32 bits encoding");
  assert(c.z < (1U << 10) &&
         "Maximum z coordinate is 2^10 - 1 for 32 bits encoding");
  return detail::morton3d<uint32_t, uint16_t, Tag>::encode(c);
}

/// @brief Encode 3D coordinates into 64-bits morton code
/// @tparam Tag Tag to switch implementations
/// @param[in] c Coordinates
/// @returns Morton code
template <typename Tag = default_tag>
inline morton_code64_t encode(const coordinates32_t& c, Tag = Tag{}) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  assert(c.x < (1UL << 21) &&
         "Maximum x coordinate is 2^21 - 1 for 64 bits encoding");
  assert(c.y < (1UL << 21) &&
         "Maximum y coordinate is 2^21 - 1 for 64 bits encoding");
  assert(c.z < (1UL << 21) &&
         "Maximum z coordinate is 2^21 - 1 for 64 bits encoding");
  return detail::morton3d<uint64_t, uint32_t, Tag>::encode(c);
}

/// @brief Decode 32-bits morton code into 3D coordinates
/// @tparam Tag Tag to switch implementation
/// @param[in] m Morton code
/// @returns Coordinates
template <typename Tag = default_tag>
inline coordinates16_t decode(const morton_code32_t m, Tag = Tag{}) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  assert(m.value < (1UL << 30) &&
         "Maximum morton code is 2^30 - 1 for 32 bits encoding");
  return detail::morton3d<uint32_t, uint16_t, Tag>::decode(m);
}

/// @brief Decode 64-bits morton code into 3D coordinates
/// @tparam Tag Tag to switch implementation
/// @param[in] m Morton code
/// @returns Coordinates
template <typename Tag = default_tag>
inline coordinates32_t decode(const morton_code64_t m, Tag = Tag{}) noexcept {
  static_assert(is_tag<Tag>::value, "Tag is not a tag type");
  assert(m.value < (1ULL << 63) &&
         "Maximum morton code is 2^63 - 1 for 64 bits encoding");
  return detail::morton3d<uint64_t, uint32_t, Tag>::decode(m);
}

}  // namespace morton3d

#endif  // MORTON_MORTON3D_HPP