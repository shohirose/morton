# Morton
[![Build Status](https://travis-ci.com/shohirose/morton.svg?branch=master)](https://travis-ci.com/shohirose/morton) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

This is a modified version of [`libmorton`](https://github.com/Forceflow/libmorton) originally created by [@Forceflow](https://github.com/Forceflow). Encoding and decoding morton codes using 

- Look-up tables
- Pre-shifted look-up tables
- BMI instruction sets
- Magic bits

in `libmorton` are implemented here for self-learning.　Unit tests are created by using [Google Test](https://github.com/google/googletest).

## Usage

Please include `morton/morton2d.hpp` or `morton/morton3d.hpp`. If `__BMI2__` or `__AVX2__` macro is defined, `encode`/`decode` functions use BMI instruction sets.

```cpp
namespace morton2d {

// 32/64 bits encoding in two dimensions.
morton_code32_t encode(const coordinates16_t& c);
morton_code64_t encode(const coordinates32_t& c);

// 32/64 bits decoding in two dimensions.
coordinates16_t decode(const morton_code32_t m);
coordinates32_t decode(const morton_code64_t m);

} // namespace morton2d
```

```cpp
namespace morton3d {

// 32/64 bits encoding in three dimensions.
morton_code32_t encode(const coordinates16_t& c);
morton_code64_t encode(const coordinates32_t& c);

// 32/64 bits decoding in three dimensions.
coordinates16_t decode(const morton_code32_t m);
coordinates32_t decode(const morton_code64_t m);

} // namespace morton3d
```

If you want to use a specific encoding/decoding implementation, you can specify the implementation using tags. A tag type must be either of the following types defined in `morton2d`/`morton3d` namespaces:

- `tag::bmi`: Implementation using BMI instruction sets.
- `tag::lookup_table`: Implementation using look-up tables.
- `tag::preshifted_lookup_table`: Implementation using pre-shifted look-up tables.
- `tag::magic_bits`: Implementation using magic bits

```cpp
namespace morton2d {

// 32/64 bits encoding in two dimensions.
template <typename Tag>
morton_code32_t encode(const coordinates16_t& c, Tag tag);
template <typename Tag>
morton_code64_t encode(const coordiantes32_t& c, Tag tag);

// 32/64 bits decoding in two dimensions.
template <typename Tag>
coordinates16_t decode(const morton_code32_t m, Tag tag);
template <typename Tag>
coordiantes32_t decode(const morton_code64_t m, Tag tag);

} // namespace morton2d
```

```cpp
namespace morton3d {

// 32/64 bits encoding in three dimensions.
template <typename Tag>
morton_code32_t encode(const coordinates16_t& c, Tag tag);
template <typename Tag>
morton_code64_t encode(const coordinates32_t& c, Tag tag);

// 32/64 bits decoding in three dimensions.
template <typename Tag>
coordinates16_t decode(const morton_code32_t m, Tag tag);
template <typename Tag>
coordinates32_t decode(const morton_code64_t m, Tag tag);

} // namespace morton3d
```

Using the above functions, you can do like:

```cpp
using namespace morton2d;
const coordinates16_t c{/* ... */};
const auto m = encode(c, tag::preshifted_lookup_table{});
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