# Morton
[![Build Status](https://travis-ci.com/shohirose/morton.svg?branch=master)](https://travis-ci.com/shohirose/morton) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

This is a modified version of [`libmorton`](https://github.com/Forceflow/libmorton) originally created by [@Forceflow](https://github.com/Forceflow). Encoding and decoding morton codes using 

- Lookup tables
- Preshifted lookup tables
- BMI instruction sets
- Magic bits

in `libmorton` are implemented here for self-learning.　Unit tests are created by using [Google Test](https://github.com/google/googletest).

## Usage

Please include `morton/morton2d.hpp` or `morton/morton3d.hpp`. You can specify an encoding/decoding implementation using a tag. A tag must be one of the following types defined in `morton2d`/`morton3d` namespaces:

- `tag::bmi`: Implementation using BMI instruction sets.
- `tag::lookup_table`: Implementation using lookup tables.
- `tag::preshifted_lookup_table`: Implementation using preshifted lookup tables.
- `tag::magic_bits`: Implementation using magic bits

If you do not specify a tag, `default_tag` is automatically used. `default_tag` is the alias of `tag::bmi` if `__BMI2__` or `__AVX2__` macro is defined, otherwise it is the alias of `tag::preshifted_lookup_table`.

```cpp
namespace morton2d {

// 32/64 bits encoding in two dimensions.
template <typename Tag = default_tag>
morton_code32_t encode(const coordinates16_t& c, Tag = Tag{});
template <typename Tag = default_tag>
morton_code64_t encode(const coordiantes32_t& c, Tag = Tag{});

// 32/64 bits decoding in two dimensions.
template <typename Tag = default_tag>
coordinates16_t decode(const morton_code32_t m, Tag = Tag{});
template <typename Tag = default_tag>
coordiantes32_t decode(const morton_code64_t m, Tag = Tag{});

} // namespace morton2d
```

```cpp
namespace morton3d {

// 32/64 bits encoding in three dimensions.
template <typename Tag = default_tag>
morton_code32_t encode(const coordinates16_t& c, Tag = Tag{});
template <typename Tag = default_tag>
morton_code64_t encode(const coordinates32_t& c, Tag = Tag{});

// 32/64 bits decoding in three dimensions.
template <typename Tag = default_tag>
coordinates16_t decode(const morton_code32_t m, Tag = Tag{});
template <typename Tag = default_tag>
coordinates32_t decode(const morton_code64_t m, Tag = Tag{});

} // namespace morton3d
```

Using the above functions, you can do like:

```cpp
using namespace morton2d;
const coordinates16_t c(2, 5);
const auto m = encode(c, tag::preshifted_lookup_table{});
```

It should be noted that coordinates (`coordiantes16_t`/`coordinates32_t`), morton codes (`morton_code32_t`/`morton_code64_t`), and the aforementioned tags are defined in `morton2d` and `morton3d` namespaces, respectively. Please do not confuse, for example, `morton2d::morton_code32_t` with `morton3d::morton_code32_t`. They are completely different types.

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

## Testing

`test` directory contains unit tests for the above functions by using Google Test.

## Benchmarking

`benchmark` directory contains benchmarks with the aid of [Google's benchmark](https://github.com/google/benchmark).

## Citation

Please follow the instruction written in [`libmorton`](https://github.com/Forceflow/libmorton).