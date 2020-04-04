// This software is released under the MIT license.
//
// Copyright (c) 2016 Jeroen Baert
// Copyright (c) 2020 Sho Hirose
#ifndef MORTON_MORTON_HPP
#define MORTON_MORTON_HPP

#if defined(__BMI2__) || __AVX2__
#define MORTON_USE_BMI
#include <immintrin.h>
#endif

#include <cstdint>

/// Moton namespace
namespace morton {

/// Tags for partial specialization of morton2d and morton3d classes.
namespace tag {

/// Tag for the implementation using pre-shifted look-up table
struct preshifted_lookup_table {};

/// Tag for the implementation using look-up table
struct lookup_table {};

/// Tag for the implementation using BMI instructions
struct bmi {};

}  // namespace tag

/// Detail implementation of morton library
namespace detail {

/// Look-up tables
namespace lookup_table {

/// For encoding in two dimensions
namespace encode2d {

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

}  // namespace encode2d

/// For decoding in two dimensions
namespace decode2d {

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

}  // namespace decode2d

/// For endocding in three dimensions
namespace encode3d {

static const uint_fast32_t x[256] = {
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

static const uint_fast32_t y[256] = {
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

static const uint_fast32_t z[256] = {
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

}  // namespace encode3d

/// For decoding in three dimensions
namespace decode3d {

static const uint_fast8_t x[512] = {
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

static const uint_fast8_t y[512] = {
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

static const uint_fast8_t z[512] = {
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

}  // namespace decode3d

}  // namespace lookup_table

/// @brief Morton code implementation in two dimensions.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
/// @tparam Tag Tag to switch implementations
template <typename MortonCode, typename Coordinate, typename Tag>
struct morton2d {};

/// @brief Morton code implementation using pre-shifted look-up tables in two
/// dimensions.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
template <typename MortonCode, typename Coordinate>
class morton2d<MortonCode, Coordinate, tag::preshifted_lookup_table> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] x X coordinate
  /// @param[in] y Y coordinate
  /// @returns Moton code
  static MortonCode encode(const Coordinate x, const Coordinate y) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @param[out] x X coordinate
  /// @param[out] y Y coordinate
  static void decode(const MortonCode m, Coordinate& x, Coordinate& y) noexcept;

 private:
  /// Helper function for decode
  /// @param[in] m Morton code
  /// @param[in] table Look-up table
  /// @returns Coordinate
  static Coordinate decode(const MortonCode m,
                           const uint_fast8_t* table) noexcept;
};

template <typename MortonCode, typename Coordinate>
inline MortonCode
morton2d<MortonCode, Coordinate, tag::preshifted_lookup_table>::encode(
    const Coordinate x, const Coordinate y) noexcept {
  MortonCode code = 0;
  // 8-bit mask
  constexpr MortonCode mask = 0x000000FF;
  for (unsigned int i = sizeof(Coordinate); i > 0; --i) {
    const unsigned int shift = (i - 1) * 8;
    code = code << 16 |  //
           lookup_table::encode2d::y[(y >> shift) & mask] |
           lookup_table::encode2d::x[(x >> shift) & mask];
  }
  return code;
}

template <typename MortonCode, typename Coordinate>
inline Coordinate
morton2d<MortonCode, Coordinate, tag::preshifted_lookup_table>::decode(
    const MortonCode m, const uint_fast8_t* table) noexcept {
  MortonCode code = 0;
  // 8-bit mask
  constexpr MortonCode mask = 0x000000FF;
  for (unsigned int i = 0; i < sizeof(MortonCode); ++i) {
    const unsigned int shift = i * 8;
    code |= static_cast<MortonCode>(table[(m >> shift) & mask] << (4 * i));
  }
  return static_cast<Coordinate>(code);
}

template <typename MortonCode, typename Coordinate>
inline void
morton2d<MortonCode, Coordinate, tag::preshifted_lookup_table>::decode(  //
    const MortonCode m, Coordinate& x, Coordinate& y) noexcept {
  x = decode(m, lookup_table::decode2d::x);
  y = decode(m, lookup_table::decode2d::y);
}

/// @brief Morton code implementation using look-up tables in two dimensions.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
template <typename MortonCode, typename Coordinate>
class morton2d<MortonCode, Coordinate, tag::lookup_table> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] x X coordinate
  /// @param[in] y Y coordinate
  /// @returns Moton code
  static MortonCode encode(const Coordinate x, const Coordinate y) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @param[out] x X coordinate
  /// @param[out] y Y coordinate
  static void decode(const MortonCode m, Coordinate& x, Coordinate& y) noexcept;

 private:
  /// Helper function for decode
  /// @param[in] m Morton code
  /// @param[in] table Look-up table
  /// @param[in] shift0 Start shift
  /// @returns Coordinate
  static Coordinate decode(const MortonCode m, const uint_fast8_t* table,
                           const unsigned int shift0) noexcept;
};

template <typename MortonCode, typename Coordinate>
inline MortonCode morton2d<MortonCode, Coordinate, tag::lookup_table>::encode(
    const Coordinate x, const Coordinate y) noexcept {
  MortonCode code = 0;
  // 8-bit mask
  constexpr MortonCode mask = 0x000000FF;
  for (unsigned int i = sizeof(Coordinate); i > 0; --i) {
    const unsigned int shift = (i - 1) * 8;
    code = code << 16 |  //
           (lookup_table::encode2d::x[(y >> shift) & mask] << MortonCode(1)) |
           lookup_table::encode2d::x[(x >> shift) & mask];
  }
  return code;
}

template <typename MortonCode, typename Coordinate>
inline Coordinate morton2d<MortonCode, Coordinate, tag::lookup_table>::decode(
    const MortonCode m, const uint_fast8_t* table,
    const unsigned int shift0) noexcept {
  MortonCode code = 0;
  // 8-bit mask
  constexpr MortonCode mask = 0x000000FF;
  for (unsigned int i = 0; i < sizeof(MortonCode); ++i) {
    const unsigned int shift = i * 8 + shift0;
    code |= static_cast<MortonCode>(table[(m >> shift) & mask] << (4 * i));
  }
  return static_cast<Coordinate>(code);
}

template <typename MortonCode, typename Coordinate>
inline void morton2d<MortonCode, Coordinate, tag::lookup_table>::decode(  //
    const MortonCode m, Coordinate& x, Coordinate& y) noexcept {
  x = decode(m, lookup_table::decode2d::x, 0);
  y = decode(m, lookup_table::decode2d::x, 1);
}

#ifdef MORTON_USE_BMI

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
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
template <typename MortonCode, typename Coordinate>
class morton2d<MortonCode, Coordinate, tag::bmi> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] x X coordinate
  /// @param[in] y Y coordinate
  /// @returns Moton code
  static MortonCode encode(const Coordinate x, const Coordinate y) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @param[out] x X coordinate
  /// @param[out] y Y coordinate
  static void decode(const MortonCode m, Coordinate& x, Coordinate& y) noexcept;
};

template <typename MortonCode, typename Coordinate>
inline MortonCode morton2d<MortonCode, Coordinate, tag::bmi>::encode(
    const Coordinate x, const Coordinate y) noexcept {
  MortonCode m = 0;
  constexpr MortonCode mask_x = 0x5555555555555555;
  constexpr MortonCode mask_y = 0xAAAAAAAAAAAAAAAA;
  m |= pdep(static_cast<MortonCode>(x), mask_x) |
       pdep(static_cast<MortonCode>(y), mask_y);
  return m;
}

template <typename MortonCode, typename Coordinate>
inline void morton2d<MortonCode, Coordinate, tag::bmi>::decode(  //
    const MortonCode m, Coordinate& x, Coordinate& y) noexcept {
  constexpr MortonCode mask_x = 0x5555555555555555;
  constexpr MortonCode mask_y = 0xAAAAAAAAAAAAAAAA;
  x = static_cast<Coordinate>(pext(m, mask_x));
  y = static_cast<Coordinate>(pext(m, mask_y));
}
#endif  // MORTON_USE_BMI

}  // namespace detail

/// @brief Encode 2D coordinates into 32-bits morton code.
/// @tparam Tag Tag to switch implementation
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @returns Morton code
template <typename Tag>
inline uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y,
                            Tag) noexcept {
  return detail::morton2d<uint_fast32_t, uint_fast16_t, Tag>::encode(x, y);
}

/// @brief Encode 2D coordinates into 66-bits morton code.
/// @tparam Tag Tag to switch implementation
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @returns Morton code
template <typename Tag>
inline uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y,
                            Tag) noexcept {
  return detail::morton2d<uint_fast64_t, uint_fast32_t, Tag>::encode(x, y);
}

/// @brief Encode 2D coordinates into 32-bits morton code.
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @returns Morton code
inline uint_fast32_t encode(const uint_fast16_t x,
                            const uint_fast16_t y) noexcept {
#ifdef MORTON_USE_BMI
  return encode(x, y, tag::bmi{});
#else
  return encode(x, y, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

/// @brief Encode 2D coordinates into 64-bits morton code.
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @returns Morton code
inline uint_fast64_t encode(const uint_fast32_t x,
                            const uint_fast32_t y) noexcept {
#ifdef MORTON_USE_BMI
  return encode(x, y, tag::bmi{});
#else
  return encode(x, y, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

/// @brief Decode 32-bits morton code into 2D coordinates.
/// @tparam Tag Tag to switch implementations
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
template <typename Tag>
inline void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y,
                   Tag) noexcept {
  detail::morton2d<uint_fast32_t, uint_fast16_t, Tag>::decode(m, x, y);
}

/// @brief Decode 64-bits morton code into 2D coordinates.
/// @tparam Tag Tag to switch implementations
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
template <typename Tag>
inline void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y,
                   Tag) noexcept {
  detail::morton2d<uint_fast64_t, uint_fast32_t, Tag>::decode(m, x, y);
}

/// @brief Decode 32-bits morton code into 2D coordinates.
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
inline void decode(const uint_fast32_t m, uint_fast16_t& x,
                   uint_fast16_t& y) noexcept {
#ifdef MORTON_USE_BMI
  decode(m, x, y, tag::bmi{});
#else
  decode(m, x, y, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

/// @brief Decode 64-bits morton code into 2D coordinates.
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
inline void decode(const uint_fast64_t m, uint_fast32_t& x,
                   uint_fast32_t& y) noexcept {
#ifdef MORTON_USE_BMI
  decode(m, x, y, tag::bmi{});
#else
  decode(m, x, y, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

namespace detail {

/// @brief Morton code implementation in three dimensions.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
/// @tparam Tag Tag to switch implementations
template <typename MortonCode, typename Coordinate, typename Tag>
class morton3d {};

/// @brief Morton code implementation using pre-shifted look-up tables in three
/// dimensions.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
template <typename MortonCode, typename Coordinate>
class morton3d<MortonCode, Coordinate, tag::preshifted_lookup_table> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] x X coordinate
  /// @param[in] y Y coordinate
  /// @param[in] z Z coordinate
  /// @returns Moton code
  static MortonCode encode(const Coordinate x, const Coordinate y,
                           const Coordinate z) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @param[out] x X coordinate
  /// @param[out] y Y coordinate
  /// @param[out] z Z coordinate
  static void decode(const MortonCode m, Coordinate& x, Coordinate& y,
                     Coordinate& z) noexcept;

 private:
  /// Helper function for decode
  /// @param[in] m Morton code
  /// @param[in] table Look-up table
  /// @returns Coordinate
  static Coordinate decode(const MortonCode m,
                           const uint_fast8_t* table) noexcept;
};

template <typename MortonCode, typename Coordinate>
inline MortonCode
morton3d<MortonCode, Coordinate, tag::preshifted_lookup_table>::encode(
    const Coordinate x, const Coordinate y, const Coordinate z) noexcept {
  MortonCode code = 0;
  // 8-bit mask
  constexpr MortonCode mask = 0x000000FF;
  for (unsigned int i = sizeof(Coordinate); i > 0; --i) {
    unsigned int shift = (i - 1) * 8;
    code = code << 24 | (lookup_table::encode3d::z[(z >> shift) & mask] |
                         lookup_table::encode3d::y[(y >> shift) & mask] |
                         lookup_table::encode3d::x[(x >> shift) & mask]);
  }
  return code;
}

template <typename MortonCode, typename Coordinate>
inline Coordinate
morton3d<MortonCode, Coordinate, tag::preshifted_lookup_table>::decode(
    const MortonCode m, const uint_fast8_t* table) noexcept {
  MortonCode code = 0;
  // ceil for 32bit, floor for 64bit
  const unsigned int loops = (sizeof(MortonCode) <= 4) ? 4 : 7;
  // 9-bit mask
  constexpr MortonCode mask = 0x000001FF;
  for (unsigned int i = 0; i < loops; ++i) {
    const unsigned int shift = i * 9;
    code |= static_cast<MortonCode>(table[(m >> shift) & mask]
                                    << MortonCode(3 * i));
  }
  return static_cast<Coordinate>(code);
}

template <typename MortonCode, typename Coordinate>
inline void
morton3d<MortonCode, Coordinate, tag::preshifted_lookup_table>::decode(
    const MortonCode m, Coordinate& x, Coordinate& y, Coordinate& z) noexcept {
  x = decode(m, lookup_table::decode3d::x);
  y = decode(m, lookup_table::decode3d::y);
  z = decode(m, lookup_table::decode3d::z);
}

/// @brief Morton code implementation using look-up tables in three dimensions.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
template <typename MortonCode, typename Coordinate>
class morton3d<MortonCode, Coordinate, tag::lookup_table> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] x X coordinate
  /// @param[in] y Y coordinate
  /// @param[in] z Z coordinate
  /// @returns Moton code
  static MortonCode encode(const Coordinate x, const Coordinate y,
                           const Coordinate z) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @param[out] x X coordinate
  /// @param[out] y Y coordinate
  /// @param[out] z Z coordinate
  static void decode(const MortonCode m, Coordinate& x, Coordinate& y,
                     Coordinate& z) noexcept;

 private:
  /// Helper function for decode
  /// @param[in] m Morton code
  /// @param[in] table Look-up table
  /// @param[in] shift0 Start shift
  /// @returns Coordinate
  static Coordinate decode(const MortonCode m, const uint_fast8_t* table,
                           const unsigned int shift0) noexcept;
};

template <typename MortonCode, typename Coordinate>
inline MortonCode morton3d<MortonCode, Coordinate, tag::lookup_table>::encode(
    const Coordinate x, const Coordinate y, const Coordinate z) noexcept {
  MortonCode code = 0;
  // 8-bit mask
  constexpr MortonCode mask = 0x000000FF;
  for (unsigned int i = sizeof(Coordinate); i > 0; --i) {
    unsigned int shift = (i - 1) * 8;
    code = code << 24 |
           ((lookup_table::encode3d::x[(z >> shift) & mask] << MortonCode(2)) |
            (lookup_table::encode3d::x[(y >> shift) & mask] << MortonCode(1)) |
            lookup_table::encode3d::x[(x >> shift) & mask]);
  }
  return code;
}

template <typename MortonCode, typename Coordinate>
inline Coordinate morton3d<MortonCode, Coordinate, tag::lookup_table>::decode(
    const MortonCode m, const uint_fast8_t* table,
    const unsigned int shift0) noexcept {
  MortonCode code = 0;
  // ceil for 32bit, floor for 64bit
  const unsigned int loops = (sizeof(MortonCode) <= 4) ? 4 : 7;
  // 9-bit mask
  constexpr MortonCode mask = 0x000001FF;
  for (unsigned int i = 0; i < loops; ++i) {
    const unsigned int shift = i * 9 + shift0;
    code |= static_cast<MortonCode>(table[(m >> shift) & mask]
                                    << MortonCode(3 * i));
  }
  return static_cast<Coordinate>(code);
}

template <typename MortonCode, typename Coordinate>
inline void morton3d<MortonCode, Coordinate, tag::lookup_table>::decode(
    const MortonCode m, Coordinate& x, Coordinate& y, Coordinate& z) noexcept {
  x = decode(m, lookup_table::decode3d::x, 0);
  y = decode(m, lookup_table::decode3d::x, 1);
  z = decode(m, lookup_table::decode3d::x, 2);
}

#ifdef MORTON_USE_BMI

/// @brief Morton code implementation using BMI instruction sets.
/// @tparam MortonCode Morton code
/// @tparam Coordinate Coordinate
template <typename MortonCode, typename Coordinate>
class morton3d<MortonCode, Coordinate, tag::bmi> {
 public:
  /// @brief Encode coordinates to morton code
  /// @param[in] x X coordinate
  /// @param[in] y Y coordinate
  /// @returns Moton code
  static MortonCode encode(const Coordinate x, const Coordinate y,
                           const Coordinate z) noexcept;

  /// @brief Decode morton code to coordinates
  /// @param[in] m Morton code
  /// @param[out] x X coordinate
  /// @param[out] y Y coordinate
  /// @param[out] z Z coordinate
  static void decode(const MortonCode m, Coordinate& x, Coordinate& y,
                     Coordinate& z) noexcept;
};

template <typename MortonCode, typename Coordinate>
inline MortonCode morton3d<MortonCode, Coordinate, tag::bmi>::encode(
    const Coordinate x, const Coordinate y, const Coordinate z) noexcept {
  MortonCode m = 0;
  constexpr MortonCode mask_x = 0x9249249249249249;
  constexpr MortonCode mask_y = 0x2492492492492492;
  constexpr MortonCode mask_z = 0x4924924924924924;
  m |= pdep(static_cast<MortonCode>(x), mask_x) |
       pdep(static_cast<MortonCode>(y), mask_y) |
       pdep(static_cast<MortonCode>(z), mask_z);
  return m;
}

template <typename MortonCode, typename Coordinate>
inline void morton3d<MortonCode, Coordinate, tag::bmi>::decode(
    const MortonCode m, Coordinate& x, Coordinate& y, Coordinate& z) noexcept {
  constexpr MortonCode mask_x = 0x9249249249249249;
  constexpr MortonCode mask_y = 0x2492492492492492;
  constexpr MortonCode mask_z = 0x4924924924924924;
  x = static_cast<coord>(pext(m, mask_x));
  y = static_cast<coord>(pext(m, mask_y));
  z = static_cast<coord>(pext(m, mask_z));
}

#endif  // MORTON_USE_BMI

}  // namespace detail

/// @brief Encode 3D coordinates into 32-bits morton code
/// @tparam Tag Tag to switch implementations
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @param[in] z Z coordinate
/// @returns Morton code
template <typename Tag>
inline uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y,
                            const uint_fast16_t z, Tag) noexcept {
  return detail::morton3d<uint_fast32_t, uint_fast16_t, Tag>::encode(x, y, z);
}

/// @brief Encode 3D coordinates into 64-bits morton code
/// @tparam Tag Tag to switch implementations
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @param[in] z Z coordinate
/// @returns Morton code
template <typename Tag>
inline uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y,
                            const uint_fast32_t z, Tag) noexcept {
  return detail::morton3d<uint_fast64_t, uint_fast32_t, Tag>::encode(x, y, z);
}

/// @brief Encode 3D coordinates into 32-bits morton code
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @param[in] z Z coordinate
/// @returns Morton code
inline uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y,
                            const uint_fast16_t z) noexcept {
#ifdef MORTON_USE_BMI
  return encode(x, y, z, tag::bmi{});
#else
  return encode(x, y, z, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

/// @brief Encode 3D coordinates into 64bits morton code
/// @param[in] x X coordinate
/// @param[in] y Y coordinate
/// @param[in] z Z coordinate
/// @returns Morton code
inline uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y,
                            const uint_fast32_t z) noexcept {
#ifdef MORTON_USE_BMI
  return encode(x, y, z, tag::bmi{});
#else
  return encode(x, y, z, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

/// @brief Decode 32-bits morton code into 3D coordinates
/// @tparam Tag Tag to switch implementation
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
/// @param[out] z Z coordinate
template <typename Tag>
inline void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y,
                   uint_fast16_t& z, Tag) noexcept {
  detail::morton3d<uint_fast32_t, uint_fast16_t, Tag>::decode(m, x, y, z);
}

/// @brief Decode 64-bits morton code into 3D coordinates
/// @tparam Tag Tag to switch implementation
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
/// @param[out] z Z coordinate
template <typename Tag>
inline void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y,
                   uint_fast32_t& z, Tag) noexcept {
  detail::morton3d<uint_fast64_t, uint_fast32_t, Tag>::decode(m, x, y, z);
}

/// @brief Decode 32-bits morton code into 3D coordinates
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
/// @param[out] z Z coordinate
inline void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y,
                   uint_fast16_t& z) noexcept {
#ifdef MORTON_USE_BMI
  decode(m, x, y, z, tag::bmi{});
#else
  decode(m, x, y, z, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

/// @brief Decode 64-bits morton code into 3D coordinates
/// @param[in] m Morton code
/// @param[out] x X coordinate
/// @param[out] y Y coordinate
/// @param[out] z Z coordinate
inline void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y,
                   uint_fast32_t& z) noexcept {
#ifdef MORTON_USE_BMI
  decode(m, x, y, z, tag::bmi{});
#else
  decode(m, x, y, z, tag::preshifted_lookup_table{});
#endif  // MORTON_USE_BMI
}

}  // namespace morton

#endif  // MORTON_MORTON_HPP
