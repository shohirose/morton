# Morton
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

This is a modified version of [`libmorton`](https://github.com/Forceflow/libmorton) originally created by [@Forceflow](https://github.com/Forceflow). Encoding and decoding morton codes using 

- Look-up tables
- Pre-shifted look-up tables
- BMI instruction sets

in `libmorton` are implemented here for self-learning.　Unit tests are created by using [Google Test](https://github.com/google/googletest).

## Usage

Please include `morton/morton.hpp`. If `__BMI2__` or `__AVX2__` macro is defined, `encode`/`decode` functions use BMI instruction sets.

```cpp
namespace morton {

// 32/64 bits encoding in two dimensions.
uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y);
uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y);

// 32/64 bits decoding in two dimensions.
void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y);
void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y);

// 32/64 bits encoding in three dimensions.
uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z);

// 32/64 bits decoding in three dimensions.
void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z);
void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);

}
```

If you want to use a specific encoding/decoding implementation, you can specify the implementation using tags. A tag type must be either of the followings:

- `morton::tag::bmi`: Implementation using BMI instruction sets.
- `morton::tag::lookup_table`: Implementation using look-up tables.
- `morton::tag::preshifted_lookup_table`: Implementation using pre-shifted look-up tables.

```cpp
namespace morton {

// 32/64 bits encoding in two dimensions.
template <typename Tag>
uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y, Tag tag);
template <typename Tag>
uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y, Tag tag);

// 32/64 bits decoding in two dimensions.
template <typename Tag>
void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y, Tag tag);
template <typename Tag>
void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y, Tag tag);

// 32/64 bits encoding in three dimensions.
template <typename Tag>
uint_fast32_t encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z, Tag tag);
template <typename Tag>
uint_fast64_t encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z, Tag tag);

// 32/64 bits decoding in three dimensions.
template <typename Tag>
void decode(const uint_fast32_t m, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z, Tag tag);
template <typename Tag>
void decode(const uint_fast64_t m, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z, Tag tag);

}
```

Using the above functions, you can do like:

```cpp
using morton::encode;
namespace tag = morton::tag;

const uint_fast16_t x = // ...
const uint_fast16_t y = // ...
const auto m = encode(x, y, tag::preshifted_lookup_table{});
```

## Testing

`test` directory contains unit tests for the above functions by using Google Test.

## Build

You can build and test by using [CMake](https://cmake.org/). Type the following commands under the project root directory.

```terminal
mkdir build
cd build
cmake ..   # Configure with make
make       # Build
ctest      # Run unit tests
```

You can use [Ninja](https://ninja-build.org/) instead of make by typing the following command.

```terminal
cmake .. -G Ninja   # Configure with Ninja
ninja               # Build with Ninja
```

## Citation

Please follow the instruction written in [`libmorton`](https://github.com/Forceflow/libmorton).